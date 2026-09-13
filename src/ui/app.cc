#include "app.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>

using namespace ftxui;

void HyperLogApp::run() {
	search_input_ = Input(&search_query_, "Search (press Enter)...");

	load_more(50);
	auto screen = ScreenInteractive::Fullscreen();

	auto renderer = Renderer([&] {
		std::vector<Element> ui_elements;
		int visible_lines = std::max(1, ftxui::Terminal::Size().dimy - 6);

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
		if (event == Event::Return) {
			reset_search();
			return true;
		}
		if (event == Event::Home) {
			scroll_pos_ = 0;
			return true;
		}
		if (event == Event::End) {
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
	int loaded = 0;
	while (loaded < count) {
		if (!generator_.next()) {
			break;
		}

		auto log = generator_.value();
		
		if (log.level == "EOF") {
			break;
		}

		bool matched = false;
		if (search_query_.empty()) {
			matched = true;
		} else if (use_regex_) {
			try {
				std::regex re(search_query_, std::regex_constants::icase);
				matched = std::regex_search(log.message, re) || std::regex_search(log.level, re);
			}
			catch (const std::regex_error&) {
				matched = false;
			}
		} else {
			matched = (log.message.find(search_query_) != std::string::npos ||
				log.level.find(search_query_) != std::string::npos);
		}

		if (matched) {
			logs_.push_back(log);
			loaded++;
		}
	}
}

void HyperLogApp::reset_search() {
	logs_.clear();
	generator_ = stream_.stream_logs(file_path_);
	scroll_pos_ = 0;
	load_more(50);
}