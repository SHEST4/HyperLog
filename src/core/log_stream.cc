#include "log_stream.h"

Generator<LogEntry> LogStream::stream_logs(std::string file_path) {
	std::ifstream file(file_path, std::ios::in);
	if (!file.is_open()) {
		co_yield LogEntry("SYSTEM", "ERROR", "Failed to open log file: " + file_path, "");
		co_return;
	} 
	std::string	line;
	while (true) {
		if (std::getline(file, line)) {
			if (line.empty()) continue;

			if (auto entry = parser_.parse(line)) {
				co_yield *entry;
			}
		} else {
			file.clear(); // Clear EOF flag
			co_yield LogEntry("SYSTEM", "EOF", "", "");
		}
	}
}
