#pragma once

#include <string>
#include <fstream>
#include "generator.h"
#include "log_entry.h"
#include "log_parser.h"

class LogStream {
private:
	LogParser parser_;

public:
	Generator<LogEntry> stream_logs(std::string file_path);
};