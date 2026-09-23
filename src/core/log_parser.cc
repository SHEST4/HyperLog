#include "log_parser.h"

std::optional<LogEntry> LogParser::parse(const std::string_view& log_line) {
	auto get_safe_string = [](simdjson::ondemand::object& obj, const std::vector<const char*>& keys, const char* def) -> std::string {
		simdjson::ondemand::value val;
		for (const char* key : keys) {
			if (obj[key].get(val) == simdjson::SUCCESS) {
				auto type = val.type().value();
				if (type == simdjson::ondemand::json_type::string) {
					return std::string(val.get_string().value());
				}
				else if (type == simdjson::ondemand::json_type::number) {
					return std::string(val.raw_json_token());
				}
			}
		}
		return def;
		};

	try {
		simdjson::padded_string padded_log_line(log_line);
		auto doc = parser_.iterate(padded_log_line);
		simdjson::ondemand::object obj;

		LogEntry entry;
		entry.raw_json = std::string(log_line); 

		if (doc.get_object().get(obj) == simdjson::SUCCESS) {
			entry.level = get_safe_string(obj, { "level", "priority" }, "UNKNOWN");
			entry.message = get_safe_string(obj, { "message", "msg" }, "No message");
			entry.timestamp = get_safe_string(obj, { "timestamp", "@timestamp", "ts", "time" }, "-");
		}
		else {
			entry.level = "RAW";
			entry.message = std::string(log_line);
			entry.timestamp = "-";
		}

		return entry;
	}
	catch (...) {
		return LogEntry("-", "RAW", std::string(log_line), std::string(log_line));
	}
}
