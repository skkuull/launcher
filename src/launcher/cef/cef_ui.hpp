#pragma once

#include <utils/nt.hpp>
#include "cef_ui_handler.hpp"

namespace cef
{
	class cef_ui
	{
	public:
		cef_ui(utils::nt::library process, std::string path);
		~cef_ui();

		HWND get_window() const;

		void close_browser();
		void reload_browser() const;

		int run_process() const;
		void create(const std::string& folder, const std::string& file);
		void work_once();
		void work();

	private:
		utils::nt::library process_;
		bool initialized_ = false;

		std::string path_;
		CefRefPtr<CefBrowser> browser_;
		CefRefPtr<cef_ui_handler> ui_handler_;

		void set_window_icon() const;

		static void invoke_close_browser(CefRefPtr<CefBrowser> browser);
	};
}
