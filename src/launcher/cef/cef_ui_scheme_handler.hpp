#pragma once

namespace cef
{
	class cef_ui_scheme_handler_factory : public CefSchemeHandlerFactory
	{
	public:
		cef_ui_scheme_handler_factory(std::string folder);

		CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
		                                     CefRefPtr<CefFrame> frame,
		                                     const CefString& scheme_name,
		                                     CefRefPtr<CefRequest> request) override;

	private:
		std::string folder_;

		IMPLEMENT_REFCOUNTING(cef_ui_scheme_handler_factory);
		DISALLOW_COPY_AND_ASSIGN(cef_ui_scheme_handler_factory);
	};
}
