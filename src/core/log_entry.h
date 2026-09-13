#pragma once

#include <string>

struct LogEntry {
	std::string timestamp;
	std::string level;
	std::string message;
	std::string raw_json;

	LogEntry() = default;
	LogEntry(const std::string& ts, const std::string& lvl, const std::string& msg, const std::string& raw)
		: timestamp(ts), level(lvl), message(msg), raw_json(raw) {}
};