#include "std_include.hpp"
#include "updater.hpp"
#include <utils/com.hpp>

namespace updater
{
	void run(const HINSTANCE instance)
	{
		auto dialog = utils::com::create_progress_dialog();

		dialog->SetTitle(L"Updating...");
		dialog->SetLine(0, L"Updating X Labs Launcher...", false, nullptr);
		dialog->SetAnimation(instance, 1001);
		dialog->StartProgressDialog(nullptr, nullptr, PROGDLG_AUTOTIME, nullptr);

		int max = 1000;
		for(int i = 0; i <= max && !dialog->HasUserCancelled(); ++i)
		{
			dialog->SetProgress(i, max);
			Sleep(10);
		}

		dialog->StopProgressDialog();
	}
}
