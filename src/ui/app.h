#pragma	once

#include <string>
#include <vector>
#include <regex>
#include <atomic>
#include <mutex>
#include <thread>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include "../core/log_stream.h"

class HyperLogApp {
private:
	LogStream stream_;
	std::vector<LogEntry> logs_;
	Generator<LogEntry> generator_;
	int scroll_pos_ = 0;
	std::string file_path_;
	std::string search_query_;
	ftxui::Component search_input_;
	bool use_regex_ = false;
	ftxui::Component regex_checkbox_;
	std::atomic<bool> is_loading_ = false;
	std::mutex logs_mutex_;
	std::jthread search_thread_;
	std::string applied_query_ = "";

	void load_more(int count);
	void reset_search();
	std::string to_lower_case(const std::string& str);

public:
	HyperLogApp(const std::string& file_path) 
		: file_path_(file_path), generator_(stream_.stream_logs(file_path)) {}
	void run();
};