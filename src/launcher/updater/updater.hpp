#pragma once

namespace updater
{
	class update_canceled : public std::runtime_error {
	public:
		update_canceled();
	};

	void run(const std::string& base);
}
