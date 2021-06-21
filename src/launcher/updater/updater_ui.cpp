#include "std_include.hpp"
#include "updater_ui.hpp"
#include "update_cancelled.hpp"

namespace updater
{
	updater_ui::updater_ui()
	{
		this->progress_ui_.set_title("X Labs Updater");
		this->progress_ui_.show();
	}

	updater_ui::~updater_ui() = default;

	void updater_ui::update_files(const std::vector<file_info>& files)
	{
		this->handle_cancellation();
	}

	void updater_ui::done_update()
	{

	}

	void updater_ui::begin_file(const file_info& file)
	{
		this->handle_cancellation();
	}

	void updater_ui::end_file(const file_info& file)
	{

	}

	void updater_ui::file_progress(const file_info& file, size_t progress)
	{
		this->handle_cancellation();
	}

	void updater_ui::handle_cancellation() const
	{
		if(this->progress_ui_.is_cancelled())
		{
			throw update_cancelled();
		}
	}


	/*
 *
 * 	void file_updater::update_files(const std::vector<file_info>& outdated_files)
	{
		const ui dialog{};
		dialog.set_title("X Labs Updater");
		dialog.show();

		size_t total_size = 0, downloaded_size = 0;
		for (const auto& info : outdated_files)
		{
			total_size += info.size;
		}

		for (size_t i = 0; i < outdated_files.size(); ++i)
		{
			if (dialog.is_cancelled())
			{
				throw update_canceled();
			}

			const auto& file = outdated_files[i];

			dialog.set_line(1, utils::string::va("Updating files... (%zu/%zu)", i,
			                                     outdated_files.size()));
			dialog.set_line(2, file.name);
			dialog.set_progress(downloaded_size, total_size);

			update_file(base, file, [&](const size_t size)
			{
				if (dialog.is_cancelled())
				{
					throw update_canceled();
				}

				dialog.set_progress(downloaded_size + size, total_size);
			});

			downloaded_size += file.size;
			dialog.set_progress(downloaded_size, total_size);
		}
	}
 */
}
