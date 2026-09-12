#pragma once

#include <string_view>
#include <optional>
#include <simdjson.h>
#include "log_entry.h"


class LogParser {
private:
	simdjson::ondemand::parser parser_;

public:
	std::optional<LogEntry> parse(const std::string_view& log_line);

};