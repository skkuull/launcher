#include "std_include.hpp"
#include "updater.hpp"
#include "updater_ui.hpp"

namespace updater
{
	void run()
	{
		const ui dialog{};
		dialog.set_title("X Labs Updater");
		dialog.set_line(1, "Updating X Labs Launcher...");
		dialog.set_line(2, "Test");

		dialog.show();

		const size_t max = 1000;
		for (size_t i = 0; i <= max && !dialog.is_cancelled(); ++i)
		{
			dialog.set_progress(i, max);
			std::this_thread::sleep_for(10ms);
		}
	}
}
