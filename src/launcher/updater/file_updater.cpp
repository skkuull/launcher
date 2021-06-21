#include "std_include.hpp"

#include "updater.hpp"
#include "updater_ui.hpp"
#include "file_updater.hpp"

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

		const file_info* find_host_file_info(const std::vector<file_info>& outdated_files)
		{
			for (const auto& file : outdated_files)
			{
				if (file.name == UPDATE_HOST_BINARY)
				{
					return &file;
				}
			}

			return nullptr;
		}
	}

	file_updater::file_updater(progress_listener& listener, std::string base, std::string process_file)
		: listener_(listener)
		, base_(std::move(base))
	    , process_file_(std::move(process_file))
	{
		this->dead_process_file_ = this->process_file_ + ".old";
		this->delete_old_process_file();
	}

	void file_updater::run() const
	{
		const auto files = get_file_infos();
		const auto outdated_files = this->get_outdated_files(files);
		if (outdated_files.empty())
		{
			return;
		}

		this->update_host_binary(outdated_files);
		this->update_files(outdated_files);
	}

	void file_updater::update_file(const file_info& file) const
	{
		const auto url = UPDATE_FOLDER + file.name;
		const auto data = utils::http::get_data(url, {}, [&](const size_t progress)
		{
			this->listener_.file_progress(file, progress);
		});
		
		if (!data || data->size() != file.size || get_hash(*data) != file.hash) {
			throw std::runtime_error("Failed to download: " + url);
		}

		const auto out_file = this->get_drive_filename(file);
		if (!utils::io::write_file(out_file, *data, false)) {
			throw std::runtime_error("Failed to write: " + file.name);
		}
	}

	std::vector<file_info> file_updater::get_outdated_files(const std::vector<file_info>& files) const
	{
		std::vector<file_info> outdated_files{};

		for (const auto& info : files)
		{
			if (this->is_outdated_file(info))
			{
				outdated_files.emplace_back(info);
			}
		}

		return outdated_files;
	}

	void file_updater::update_host_binary(const std::vector<file_info>& outdated_files) const
	{
		const auto* host_file = find_host_file_info(outdated_files);
		if (!host_file)
		{
			return;
		}
		
		try
		{
			this->move_current_process_file();
			this->update_files({*host_file});
		}
		catch(...)
		{
			this->restore_current_process_file();
			throw;
		}

		utils::nt::relaunch_self();
		throw update_cancelled();
	}

	void file_updater::update_files(const std::vector<file_info>& outdated_files) const
	{
		this->listener_.update_files(outdated_files);
		
		for (const auto& file : outdated_files)
		{
			this->listener_.begin_file(file);
			this->update_file(file);
			this->listener_.end_file(file);
		}

		this->listener_.done_update();
	}

	bool file_updater::is_outdated_file(const file_info& file) const
	{
		std::string data{};
		const auto drive_name = this->get_drive_filename(file);
		if (!utils::io::read_file(drive_name, &data)) {
			return true;
		}

		if (data.size() != file.size) {
			return true;
		}

		const auto hash = get_hash(data);
		return hash != file.hash;
	}

	std::string file_updater::get_drive_filename(const file_info& file) const
	{
		if (file.name == UPDATE_HOST_BINARY) {
			return this->process_file_;
		}

		return this->base_ + file.name;
	}

	void file_updater::move_current_process_file() const
	{
		utils::io::move_file(this->process_file_, this->dead_process_file_);
	}

	void file_updater::restore_current_process_file() const
	{
		utils::io::move_file(this->dead_process_file_, this->process_file_);
	}

	void file_updater::delete_old_process_file() const
	{
		// Wait for other process to die
		for (auto i = 0; i < 4; ++i)
		{
			utils::io::remove_file(this->dead_process_file_);
			if (!utils::io::file_exists(this->dead_process_file_))
			{
				break;
			}

			std::this_thread::sleep_for(2s);
		}
	}
}
