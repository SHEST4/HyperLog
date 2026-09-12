#include "log_parser.h"

std::optional<LogEntry> LogParser::parse(const std::string_view& log_line) {
	try {
		simdjson::padded_string padded_log_line(log_line);
		auto doc = parser_.iterate(padded_log_line);
		
		std::string_view timestamp, level, message;
		timestamp = doc["timestamp"].get_string().value();
		level = doc["level"].get_string().value();
		message = doc["message"].get_string().value();

		return LogEntry{
			std::string(timestamp),
			std::string(level),
			std::string(message),
			std::string(log_line)
		};
	} catch (const simdjson::simdjson_error& e) {
		return std::nullopt;
	}
}
