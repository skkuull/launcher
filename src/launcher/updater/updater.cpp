#include "std_include.hpp"

#include "updater.hpp"
#include "updater_ui.hpp"

#include <utils/cryptography.hpp>
#include <utils/http.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

#define UPDATE_SERVER "https://master.xlabs.dev/"
#define UPDATE_FILE UPDATE_SERVER "files.json"
#define UPDATE_FOLDER UPDATE_SERVER "data/"
#define UPDATE_HOST_BINARY "xlabs.exe"

namespace updater
{
	namespace
	{
		struct file_info
		{
			std::string name;
			size_t size;
			std::string hash;
		};

		std::vector<file_info> parse_file_infos(const std::string& json)
		{
			rapidjson::Document doc{};
			doc.Parse(json.data(), json.size());

			if (!doc.IsArray())
			{
				return {};
			}

			std::vector<file_info> files{};

			for (const auto& element : doc.GetArray())
			{
				if (!element.IsArray())
				{
					continue;
				}

				auto array = element.GetArray();

				file_info info{};
				info.name.assign(array[0].GetString(), array[0].GetStringLength());
				info.size = array[1].GetInt64();
				info.hash.assign(array[2].GetString(), array[2].GetStringLength());

				files.emplace_back(std::move(info));
			}

			return files;
		}

		std::vector<file_info> get_file_infos()
		{
			const auto json = utils::http::get_data(UPDATE_FILE);
			if (!json)
			{
				return {};
			}

			return parse_file_infos(*json);
		}

		std::string get_hash(const std::string& data)
		{
			return utils::cryptography::sha1::compute(data, true);
		}

		std::string get_drive_filename(const std::string& base, const file_info& info, const std::string& self_file)
		{
			auto out_file = self_file;
			if (info.name != UPDATE_HOST_BINARY)
			{
				out_file = base + info.name;
			}

			return out_file;
		}

		bool is_outdated_file(const std::string& base, const file_info& info, const std::string& self_file)
		{
			std::string data{};
			if (!utils::io::read_file(get_drive_filename(base, info, self_file), &data))
			{
				return true;
			}

			if (data.size() != info.size)
			{
				return true;
			}

			const auto hash = get_hash(data);
			return hash != info.hash;
		}

		std::vector<file_info> get_outdated_files(const std::string& base,
		                                          const std::vector<file_info>& infos, const std::string& self_file)
		{
			std::vector<file_info> files{};

			for (const auto& info : infos)
			{
				if (is_outdated_file(base, info, self_file))
				{
					files.emplace_back(info);
				}
			}

			return files;
		}


		void update_file(const std::string& base, const file_info& info,
		                 const std::function<void(size_t)>& callback)
		{
			const auto url = UPDATE_FOLDER + info.name;
			const auto data = utils::http::get_data(url, {}, callback);
			if (!data)
			{
				throw std::runtime_error("Failed to download: " + url);
			}

			if (data->size() != info.size || get_hash(*data) != info.hash)
			{
				throw std::runtime_error("Downloaded file is invalid: " + url);
			}

			auto out_file = info.name;
			if (out_file != UPDATE_HOST_BINARY)
			{
				out_file = base + info.name;
			}

			if (!utils::io::write_file(out_file, *data, false))
			{
				throw std::runtime_error("Failed to write: " + info.name);
			}
		}

		void update_files(const std::string& base, const std::vector<file_info>& outdated_files)
		{
			const ui dialog{};
			dialog.set_title("X Labs Updater");
			dialog.show();

			size_t total_size = 0, downloaded_size = 0;
			for (const auto& info : outdated_files)
			{
				total_size += info.size;
			}

			for (size_t i = 0; i < outdated_files.size(); ++i)
			{
				if (dialog.is_cancelled())
				{
					throw update_canceled();
				}

				const auto& file = outdated_files[i];

				dialog.set_line(1, utils::string::va("Updating files... (%zu/%zu)", i,
				                                     outdated_files.size()));
				dialog.set_line(2, file.name);
				dialog.set_progress(downloaded_size, total_size);

				update_file(base, file, [&](const size_t size)
				{
					if (dialog.is_cancelled())
					{
						throw update_canceled();
					}

					dialog.set_progress(downloaded_size + size, total_size);
				});

				downloaded_size += file.size;
				dialog.set_progress(downloaded_size, total_size);
			}
		}

		void delete_old_file(const std::string& file)
		{
			// Wait for other process to die
			for (auto i = 0; i < 4; ++i)
			{
				utils::io::remove_file(file);

				if (utils::io::file_exists(file))
				{
					std::this_thread::sleep_for(2s);
				}
				else
				{
					break;
				}
			}
		}

		void update_host_binary(const std::vector<file_info>& outdated_files, const std::string& self_file,
		                        const std::string& dead_file)
		{
			for (const auto& file : outdated_files)
			{
				if (file.name != UPDATE_HOST_BINARY)
				{
					continue;
				}

				utils::io::move_file(self_file, dead_file);
				update_files(self_file, {file});
				utils::nt::relaunch_self();
				throw update_canceled();
			}
		}
	}

	update_canceled::update_canceled() : std::runtime_error({})
	{
	}

	void run(const std::string& base)
	{
		const utils::nt::library self;
		const auto self_file = self.get_path();
		const auto dead_file = self_file + ".old";

		delete_old_file(dead_file);

		const auto infos = get_file_infos();
		const auto outdated_files = get_outdated_files(base, infos, self_file);
		if (outdated_files.empty())
		{
			return;
		}

		update_host_binary(outdated_files, self_file, dead_file);
		update_files(base, outdated_files);

		std::this_thread::sleep_for(1s);
	}
}
