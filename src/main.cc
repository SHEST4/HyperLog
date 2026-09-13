#include "ui/app.h"
#include <iostream>
#include <filesystem>

int main(int argc, const char* argv[]) {
   
	std::string file_path = "test.json";

	if (argc > 1) {
		file_path = argv[1];
	}

	HyperLogApp app(file_path);
	app.run();

    return (0);
}