#include "std_include.hpp"
#include "cef/cef_ui.hpp"
#include "updater.hpp"

namespace
{
	int show_window(const utils::nt::library& process)
	{
		cef::cef_ui cef_ui{process};

		{
			const auto result = cef_ui.run_process();
			if (result >= 0) return result;

			cef_ui.create("http://localhost/site/main.html");
			//cef_ui.create("file:///D:/UserData/Documents/Visual%20Studio%202013/Projects/launcher/src/launcher/resource/site/main.html");
		}

		cef_ui.work();
		return 0;
	}
}

int CALLBACK WinMain(const HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	try
	{
		updater::run();
		return show_window(utils::nt::library{instance});
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
	}

	return 1;
}
