#pragma once

#include <string>

namespace updater
{
	struct file_info
	{
		std::string name;
		size_t size;
		std::string hash;
	};
}
