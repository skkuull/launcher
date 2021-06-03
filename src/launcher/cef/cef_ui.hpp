#pragma once

#pragma warning(push)
#pragma warning(disable: 4100)

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_command_line.h"
#include "include/cef_frame.h"
#include "include/cef_web_plugin.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#pragma warning(pop)

#include <utils/nt.hpp>
#include "cef_ui_handler.hpp"

namespace cef
{
	class cef_ui
	{
	public:
		cef_ui(utils::nt::library process);
		~cef_ui();

		HWND get_window() const;

		void close_browser();
		void reload_browser() const;

		int run_process() const;
		void create(const std::string& url);
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
