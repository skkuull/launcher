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

namespace updater {
namespace {
struct file_info {
  std::string name;
  size_t size;
  std::string hash;
};

std::vector<file_info> parse_file_infos(const std::string &json) {
  rapidjson::Document doc{};
  doc.Parse(json.data(), json.size());

  if (!doc.IsArray()) {
    return {};
  }

  std::vector<file_info> files{};

  for (const auto &element : doc.GetArray()) {
    if (!element.IsArray()) {
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

std::vector<file_info> get_file_infos() {
  const auto json = utils::http::get_data(UPDATE_FILE);
  if (!json) {
    return {};
  }

  return parse_file_infos(*json);
}

std::string get_hash(const std::string &data) {
  return utils::cryptography::sha1::compute(data, true);
}

bool is_outdated_file(const std::string &base, const file_info &info) {
  std::string data{};
  if (!utils::io::read_file(base + info.name, &data)) {
    return true;
  }

  if (data.size() != info.size) {
    return true;
  }

  const auto hash = get_hash(data);
  return hash != info.hash;
}

std::vector<file_info> get_outdated_files(const std::string &base,
                                          const std::vector<file_info> &infos) {
  std::vector<file_info> files{};

  for (const auto &info : infos) {
    if (is_outdated_file(base, info)) {
      files.emplace_back(info);
    }
  }

  return files;
}
} // namespace

void update_file(const std::string &base, const file_info &info,
                 const std::function<void(size_t)> &callback) {
  const auto url = UPDATE_FOLDER + info.name;
  const auto data = utils::http::get_data(url, {}, callback);
  if (!data) {
    throw std::runtime_error("Failed to download: " + url);
  }

  if (data->size() != info.size || get_hash(*data) != info.hash) {
    throw std::runtime_error("Downloaded file is invalid: " + url);
  }

  if (!utils::io::write_file(base + info.name, *data, false)) {
    throw std::runtime_error("Failed to write: " + info.name);
  }
}

update_canceled::update_canceled() : std::runtime_error({}) {}

void run(const std::string &base) {
  const auto infos = get_file_infos();
  const auto outdated_files = get_outdated_files(base, infos);
  if (outdated_files.empty()) {
    return;
  }

  {
    const ui dialog{};
    dialog.set_title("X Labs Updater");
    dialog.show();

    size_t total_size = 0, downloaded_size = 0;
    for (const auto &info : outdated_files) {
      total_size += info.size;
    }

    for (size_t i = 0; i < outdated_files.size(); ++i) {
      if (dialog.is_cancelled()) {
        throw update_canceled();
      }

      const auto &file = outdated_files[i];

      dialog.set_line(1, utils::string ::va("Updating files (%zu/%zu)...", i,
                                            outdated_files.size()));
      dialog.set_line(2, file.name);
      dialog.set_progress(downloaded_size, total_size);

      update_file(base, file, [&](const size_t size) {
        if (dialog.is_cancelled()) {
          throw update_canceled();
        }

         dialog.set_progress(downloaded_size + size, total_size);
      });

      downloaded_size += file.size;
      dialog.set_progress(downloaded_size, total_size);
    }
  }

  std::this_thread::sleep_for(1s);
}
} // namespace updater
