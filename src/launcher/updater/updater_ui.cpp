#include "std_include.hpp"
#include "updater_ui.hpp"

#include <utils/string.hpp>

namespace updater
{
	ui::ui()
	{
		this->dialog_ = utils::com::create_progress_dialog();
		if(!this->dialog_)
		{
			throw std::runtime_error{"Failed to create dialog"};
		}
	}

	ui::~ui()
	{
		this->dialog_->StopProgressDialog();
	}

	void ui::show() const
	{
		this->dialog_->StartProgressDialog(nullptr, nullptr, PROGDLG_AUTOTIME, nullptr);
	}

	void ui::set_progress(const size_t current, const size_t max) const
	{
		this->dialog_->SetProgress64(current, max);
	}

	void ui::set_line(const int line, const std::string& text) const
	{
		this->dialog_->SetLine(line, utils::string::convert(text).data(), false, nullptr);
	}

	void ui::set_title(const std::string& title) const
	{
		this->dialog_->SetTitle(utils::string::convert(title).data());
	}

	bool ui::is_cancelled() const
	{
		return this->dialog_->HasUserCancelled();
	}
}
