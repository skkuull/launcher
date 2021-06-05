#include "std_include.hpp"
#include "cef/cef_ui.hpp"
#include "updater.hpp"

#include "utils/string.hpp"

namespace
{
	bool is_subprocess()
	{
		return strstr(GetCommandLineA(), "--xlabs-subprocess");
	}

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

	std::string get_appdata_path()
	{
		PWSTR path;
		if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path)))
		{
			throw std::runtime_error("Failed to read APPDATA path!");
		}

		auto _ = gsl::finally([&path]()
		{
			CoTaskMemFree(path);
		});

		return utils::string::convert(path) + "/xlabs";
	}

	void set_working_directory()
	{
		const auto appdata = get_appdata_path();
		SetCurrentDirectoryA(appdata.data());
	}

	void enable_dpi_awareness()
	{
		const utils::nt::library user32{"user32.dll"};
		const auto set_dpi = user32
			                     ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>(
				                     "SetProcessDpiAwarenessContext")
			                     : nullptr;
		if (set_dpi)
		{
			set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		}
	}
}

int CALLBACK WinMain(const HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	try
	{
		enable_dpi_awareness();

		if (!is_subprocess())
		{
			updater::run(instance);
		}

		return show_window(utils::nt::library{instance});
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
	}

	return 1;
}
