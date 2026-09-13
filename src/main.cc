#include "ui/app.h"
#include <iostream>
#include <filesystem>

int main(int argc, const char* argv[]) {
   
	HyperLogApp app("test.json");
	app.run();

    return (0);
}