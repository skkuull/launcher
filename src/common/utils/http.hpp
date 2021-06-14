#pragma once

#include <string>
#include <optional>
#include <future>

namespace utils::http
{
	std::optional<std::string> get_data(const std::string& url, const std::function<void(size_t)>& callback = {});
	std::future<std::optional<std::string>> get_data_async(const std::string& url);
}
