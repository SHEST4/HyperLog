#include "log_parser.h"

std::optional<LogEntry> LogParser::parse(const std::string_view& log_line) {
	try {
		simdjson::padded_string padded_log_line(log_line);
		auto doc = parser_.iterate(padded_log_line);

		std::string_view ts = "-", lvl = "INFO", msg = "";

		for (auto field : doc.get_object()) {
			std::string_view key = field.unescaped_key();
			if (key == "timestamp" || key == "time" || key == "ts" || key == "date" || key == "datetime" || key == "created_at") {
				ts = field.value().get_string().value();
			} else if (key == "level" || key == "lvl" || key == "severity" || key == "priority") {
				lvl = field.value().get_string().value();
			} else if (key == "message" || key == "msg" || key == "text" || key == "description") {
				msg = field.value().get_string().value();
			}
		}

		return LogEntry{
			std::string(ts),
			std::string(lvl),
			std::string(msg),
			std::string(log_line)
		};
	} catch (const simdjson::simdjson_error& e) {
		return std::nullopt;
	}
}
