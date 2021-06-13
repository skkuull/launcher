#pragma once

namespace cef
{
	class cef_ui_handler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler,
	                       public CefContextMenuHandler
	{
	public:
		explicit cef_ui_handler();
		~cef_ui_handler() override;

		CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
		{
			return this;
		}

		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
		{
			return this;
		}

		CefRefPtr<CefLoadHandler> GetLoadHandler() override
		{
			return this;
		}

		CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override
		{
			return this;
		}

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

		bool DoClose(CefRefPtr<CefBrowser> browser) override;

		void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                         CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;
		bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                          CefRefPtr<CefContextMenuParams> params, int command_id,
		                          CefContextMenuHandler::EventFlags event_flags) override;
		void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;
		bool RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
		                    CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model,
		                    CefRefPtr<CefRunContextMenuCallback> callback) override;

		bool is_closed(CefRefPtr<CefBrowser> browser);

	private:
		std::vector<CefRefPtr<CefBrowser>> browser_list;

	IMPLEMENT_REFCOUNTING(cef_ui_handler);
	};
}
