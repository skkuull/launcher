#pragma once

#include "named_mutex.hpp"
#include <mutex>
#include <optional>

namespace utils::properties
{
	std::unique_lock<named_mutex> lock();

	std::optional<std::string> load(const std::string& name);
	void store(const std::string& name, const std::string& value);
}
