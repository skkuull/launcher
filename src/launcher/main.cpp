#include "std_include.hpp"
#include "cef/cef_ui.hpp"
#include "updater/updater.hpp"

#include <utils/named_mutex.hpp>
#include <utils/string.hpp>

namespace
{
	bool is_subprocess()
	{
		return strstr(GetCommandLineA(), "--xlabs-subprocess");
	}

	int run_subprocess(const utils::nt::library& process, const std::string& path)
	{
		const cef::cef_ui cef_ui{process, path};
		return cef_ui.run_process();
	}

	void add_commands(cef::cef_ui& cef_ui)
	{
		cef_ui.add_command("close", [&cef_ui](const auto&, auto&)
		{
			cef_ui.close_browser();
		});

		cef_ui.add_command("minimize", [&cef_ui](const auto&, auto&)
		{
			ShowWindow(cef_ui.get_window(), SW_MINIMIZE);
		});

		cef_ui.add_command("show", [&cef_ui](const auto&, auto&)
		{
			ShowWindow(cef_ui.get_window(), SW_SHOWDEFAULT);
		});

		cef_ui.add_command("test", [](const rapidjson::Value& /*value*/, rapidjson::Document& /*response*/)
		{

		});
	}

	void show_window(const utils::nt::library& process, const std::string& path)
	{
		cef::cef_ui cef_ui{process, path};
		add_commands(cef_ui);
		cef_ui.create(path + "data/launcher-ui", "main.html");
		cef_ui.work();
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

		return utils::string::convert(path) + "/xlabs/";
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

	void run_as_singleton()
	{
		static named_mutex mutex{"xlabs-launcher"};
		if (!mutex.try_lock(1s))
		{
			throw std::runtime_error{"X Labs launcher is already running"};
		}
	}
}

int CALLBACK WinMain(const HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	try
	{
		//set_working_directory();

		const utils::nt::library lib{instance};
		const auto path = get_appdata_path();

		if (is_subprocess())
		{
			return run_subprocess(lib, path);
		}

		run_as_singleton();
		enable_dpi_awareness();
		updater::run(path);
		show_window(lib, path);
		return 0;
	}
	catch (updater::update_cancelled&)
	{
		return 0;
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "An unknown error occurred", "ERROR", MB_ICONERROR);
	}

	return 1;
}
