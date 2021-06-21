#include "std_include.hpp"
#include "updater_ui.hpp"
#include "update_cancelled.hpp"

#include <utils/string.hpp>

namespace updater
{
	updater_ui::updater_ui()
	{
	}

	updater_ui::~updater_ui() = default;

	void updater_ui::update_files(const std::vector<file_info>& files)
	{
		this->handle_cancellation();

		this->initialize_sizes(files);

		this->progress_ui_ = {};
		this->progress_ui_.set_title("X Labs Updater");
		this->progress_ui_.show();
	}

	void updater_ui::done_update()
	{
	}

	void updater_ui::begin_file(const file_info& file)
	{
		this->handle_cancellation();

		++this->downloaded_files_;

		this->progress_ui_.set_line(1, utils::string::va("Updating files... (%zu/%zu)", this->downloaded_files_,
		                                                 this->total_files_));
		this->progress_ui_.set_line(2, file.name);
		this->progress_ui_.set_progress(downloaded_size_, total_size_);
	}

	void updater_ui::end_file(const file_info& file)
	{
		this->downloaded_size_ += file.size;
		this->progress_ui_.set_progress(this->downloaded_size_, this->total_size_);
	}

	void updater_ui::file_progress(const file_info& /*file*/, const size_t progress)
	{
		this->handle_cancellation();

		this->progress_ui_.set_progress(this->downloaded_size_ + progress, this->total_size_);
	}

	void updater_ui::handle_cancellation() const
	{
		if (this->progress_ui_.is_cancelled())
		{
			throw update_cancelled();
		}
	}

	void updater_ui::initialize_sizes(const std::vector<file_info>& files)
	{
		this->downloaded_files_ = 0;
		this->downloaded_size_ = 0;

		this->total_files_ = files.size();
		this->total_size_ = 0;

		for (const auto& info : files)
		{
			this->total_size_ += info.size;
		}
	}
}
