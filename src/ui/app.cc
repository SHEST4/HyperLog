#include "app.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

using namespace ftxui;

void HyperLogApp::run() {
	for (int i = 0; i < 20; ++i) {
		if (generator_.next()) {
			logs_.push_back(generator_.value());
		} else {
			break;
		}
	}

	std::vector<Element> ui_elements;

	for (auto& log : logs_) {
		Element line;
		if (log.level == "ERROR") {
			line = hbox({
				text(log.timestamp) | dim,
				text(" [" + log.level + "] ") | color(Color::Red),
				text(log.message)
			});
		} else if (log.level == "WARN") {
			line = hbox({
				text(log.timestamp) | dim,
				text(" [" + log.level + "] ") | color(Color::Yellow),
				text(log.message)
			});
		} else if (log.level == "INFO") {
			line = hbox({
				text(log.timestamp) | dim,
				text(" [" + log.level + "] ") | color(Color::Blue),
				text(log.message)
			});
		} else if (log.level == "DEBUG") {
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
	Element document = border(vbox(ui_elements));
	auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
	ftxui::Render(screen, document);
	screen.Print();
}
