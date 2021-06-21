#pragma once

#include "progress_listener.hpp"

namespace updater
{
	class file_updater
	{
	public:
		file_updater(progress_listener& listener, std::string base, std::string process_file);

		void run() const;

		std::vector<file_info> get_outdated_files(const std::vector<file_info>& files) const;

		void update_host_binary(const std::vector<file_info>& outdated_files) const;
		void update_files(const std::vector<file_info>& outdated_files) const;

	private:
		progress_listener& listener_;
		
		std::string base_;
		std::string process_file_;
		std::string dead_process_file_;
		
		void update_file(const file_info& file) const;

		bool is_outdated_file(const file_info& file) const;
		std::string get_drive_filename(const file_info& file) const;

		void move_current_process_file() const;
		void restore_current_process_file() const;
		void delete_old_process_file() const;
	};
}
