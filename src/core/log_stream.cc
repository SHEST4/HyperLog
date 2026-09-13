#include "log_stream.h"

Generator<LogEntry> LogStream::stream_logs(std::string file_path) {
	std::ifstream file(file_path, std::ios::in);
	if (!file.is_open()) {
		co_yield LogEntry("SYSTEM", "ERROR", "Failed to open log file: " + file_path, "");
	} else {
		std::string line;
		int line_number = 1;
		while (std::getline(file, line)) {
			if (line.empty()) {
				++line_number;
				continue;
			}

			if (auto entry = parser_.parse(line)) {
				co_yield *entry;
			} else {
				co_yield LogEntry("SYSTEM", "WARN", "Failed to parse log entry at line " + std::to_string(line_number) + ": " + line, line);
			}
			++line_number;
		}
	}
}
