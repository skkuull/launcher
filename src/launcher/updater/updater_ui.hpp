#pragma once

#include "progress_ui.hpp"
#include "progress_listener.hpp"

namespace updater
{
	class updater_ui : public progress_listener
	{
	public:
		updater_ui();
		~updater_ui();

	private:
		size_t total_files_{};
		size_t downloaded_files_{};

		size_t total_size_{};
		size_t downloaded_size_{};

		progress_ui progress_ui_{};

		void update_files(const std::vector<file_info>& files) override;
		void done_update() override;

		void begin_file(const file_info& file) override;
		void end_file(const file_info& file) override;

		void file_progress(const file_info& file, size_t progress) override;

		void handle_cancellation() const;

		void initialize_sizes(const std::vector<file_info>& files);
	};
}
