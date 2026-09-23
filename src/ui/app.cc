#include "app.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>

using namespace ftxui;

void HyperLogApp::run() {
	search_input_ = Input(&search_query_, "Search (press Enter)...");

	load_more(100);
	auto screen = ScreenInteractive::Fullscreen();

	auto renderer = Renderer([&] {
		if (is_loading_) {
			return center(text("Searching, please wait..."));
		}

		std::vector<Element> ui_elements;
		int visible_lines = std::max(1, ftxui::Terminal::Size().dimy - 6);
		size_t current_size = 0;

		{
			std::lock_guard<std::mutex> lock(logs_mutex_);
			current_size = logs_.size();
			int end_pos = std::min(scroll_pos_ + visible_lines, static_cast<int>(logs_.size()));
			for (int i = scroll_pos_; i < end_pos; ++i) {
				const auto& log = logs_[i];
				Element line;
				if (log.level == "ERROR") {
					line = hbox({
						text(log.timestamp) | dim,
						text(" [" + log.level + "] ") | color(Color::Red),
						text(log.message)
						});
				}
				else if (log.level == "WARN") {
					line = hbox({
						text(log.timestamp) | dim,
						text(" [" + log.level + "] ") | color(Color::Yellow),
						text(log.message)
						});
				}
				else if (log.level == "INFO") {
					line = hbox({
						text(log.timestamp) | dim,
						text(" [" + log.level + "] ") | color(Color::Blue),
						text(log.message)
						});
				}
				else if (log.level == "DEBUG") {
					line = hbox({
						text(log.timestamp) | dim,
						text(" [" + log.level + "] ") | color(Color::Cyan),
						text(log.message)
						});
				}
				else {
					line = hbox({
						text(log.timestamp) | dim,
						text(" [" + log.level + "] ") | color(Color::Green),
						text(log.message)
						});
				}
				ui_elements.push_back(line);
			}
			return border(vbox(ui_elements));
		}
	});

	regex_checkbox_ = Checkbox("Regex", &use_regex_);

	auto layout = Container::Vertical({
		Container::Horizontal({
			search_input_,
			regex_checkbox_,
		}),
		renderer
	});

	auto main_renderer = Renderer(layout, [&] {
		size_t current_size = 0;
		{
			std::lock_guard<std::mutex> lock(logs_mutex_);
			current_size = logs_.size(); 
		}

		Element status_bar = hbox({
			text("File: " + file_path_ + " ") | bold,
			separator(),
			text(" Loaded: " + std::to_string(logs_.size()) + " ") | dim,
			separator(),
			text(" Pos: " + std::to_string(scroll_pos_) + " "),
			filler(),
			text(" [\u2191\u2193] Scroll | [F5] Live | [F12] Clear| [Ctrl+S] Export| [Enter] Search | [Esc] Exit") | dim
		}) | inverted;
		
		return vbox({
			hbox({
				text("Search: "),
				search_input_->Render() |flex,
				text(" "),
				regex_checkbox_->Render()
			}) | border,
			renderer->Render() | flex,
			status_bar
		});
	});

	auto component = CatchEvent(main_renderer, [&](Event event) {
		if (is_loading_) {
			return true;
		}
		if (event == Event::Return) {
			is_loading_ = true;
			applied_query_ = search_query_;
			{
				std::lock_guard<std::mutex> lock(logs_mutex_);
				logs_.clear();
				scroll_pos_ = 0;
			}

			search_thread_ = std::jthread([&, is_regex = use_regex_](std::stop_token stoken) {
				std::regex re;
				bool regex_valid = false;

				std::string query_lower = to_lower_case(applied_query_);
				
				if (is_regex && !applied_query_.empty()) {
					try {
						re = std::regex(applied_query_, std::regex_constants::icase);
						regex_valid = true;
					} catch (...) {}
				}

				std::vector<LogEntry> temp_logs;
				int loaded = 0;

				generator_ = stream_.stream_logs(file_path_);

				while (loaded < 100) {
					if (stoken.stop_requested()) break;

					if (!generator_.next()) break;
					auto log = generator_.value();
					if (log.level == "__HYPERLOG_EOF__") break;

					bool matched = false;
					if (applied_query_.empty()) {
						matched = true;
					}
					else if (regex_valid) {
						matched = std::regex_search(log.message, re) || std::regex_search(log.level, re);
					}
					else {
						matched = (log.message.find(applied_query_) != std::string::npos ||
							log.level.find(applied_query_) != std::string::npos);
					}

					if (matched) {
						temp_logs.push_back(log);
						loaded++;
					}
				}

				if (!stoken.stop_requested()) {
					std::lock_guard<std::mutex> lock(logs_mutex_);
					for (auto& entry : temp_logs) {
						logs_.push_back(std::move(entry));
					}
				}

				is_loading_ = false;
				screen.PostEvent(Event::Custom); 
			});
			return true;
		}

		if (event == Event::PageUp) {
			scroll_pos_ = 0;
			return true;
		}
		if (event == Event::PageDown) {
			scroll_pos_ = std::max(0, static_cast<int>(logs_.size()) - 1);
			return true;
		}
		if (event == Event::ArrowDown) {
			int visible_lines = std::max(1, ftxui::Terminal::Size().dimy - 6);

			if (scroll_pos_ + visible_lines >= static_cast<int>(logs_.size())) {
				load_more(visible_lines);
			} 
			if (scroll_pos_ < static_cast<int>(logs_.size() - 1)) {
				scroll_pos_++;
			}
			return true;
		} 
		if (event == Event::ArrowUp) {
			if (scroll_pos_ > 0) {
				scroll_pos_--;
			}
			return true;
		}
		if (event == Event::F5) {
			int visible_lines = std::max(1, ftxui::Terminal::Size().dimy - 6);
			load_more(visible_lines * 3);
			scroll_pos_ = std::max(0, static_cast<int>(logs_.size()) - 1);
			return true;
		}
		if (event == Event::F12) {
			logs_.clear();
			logs_.shrink_to_fit();
			scroll_pos_ = 0;
			return true;
		}
		if (event == Event::CtrlS) {
			std::ofstream out("export_hyperlog.json");
			for (const auto& log : logs_) {
				out << log.raw_json << "\n";
			}
			out.close();
			return true;
		}
		if (event == Event::Escape) {
			screen.ExitLoopClosure()();
			return true;
		}

		return false;
	});

	screen.Loop(component);
}

void HyperLogApp::load_more(int count) {
	std::regex re;
	bool use_regex_local = false;

	std::string query_lower = to_lower_case(applied_query_);

	if (use_regex_ && !applied_query_.empty()) {
		try {
			re = std::regex(applied_query_, std::regex_constants::icase);
			use_regex_local = true;
		}
		catch (...) {}
	}

	int loaded = 0;
	std::vector<LogEntry> temp_logs;

	while (loaded < count) {
		if (!generator_.next()) {
			break;
		}

		auto log = generator_.value();
		if (log.level == "__HYPERLOG_EOF__") {
			break;
		}

		bool matched = false;
		if (applied_query_.empty()) {
			matched = true;
		}
		else if (use_regex_local) {
			matched = std::regex_search(log.message, re) || std::regex_search(log.level, re);
		}
		else {
			std::string level_lower = to_lower_case(log.level);
			std::string msg_lower = to_lower_case(log.message);

			matched = (msg_lower.find(query_lower) != std::string::npos ||
				level_lower.find(query_lower) != std::string::npos);
		}

		if (matched) {
			temp_logs.push_back(std::move(log));
			loaded++;
		}
	}

	if (!temp_logs.empty()) {
		std::lock_guard<std::mutex> lock(logs_mutex_);
		for (auto& entry : temp_logs) {
			logs_.push_back(std::move(entry));
		}
	}
}

void HyperLogApp::reset_search() {
	logs_.clear();
	generator_ = stream_.stream_logs(file_path_);
	scroll_pos_ = 0;
	load_more(100);
}

std::string HyperLogApp::to_lower_case(const std::string& str) {
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return result;
}