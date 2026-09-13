#pragma	once

#include <string>
#include <vector>
#include "../core/log_stream.h"

class HyperLogApp {
private:
	LogStream stream_;
	std::vector<LogEntry> logs_;
	Generator<LogEntry> generator_;

public:
	HyperLogApp(const std::string& file_path) : generator_(stream_.stream_logs(file_path)) {}
	void run();
};