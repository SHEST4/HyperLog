# HyperLog

A lightning-fast, cross-platform terminal UI (TUI) JSON log viewer built for handling massive log files without consuming all your RAM. 

HyperLog uses modern C++20 coroutines for lazy file reading and vector instructions (`simdjson`) for parsing, making it capable of processing millions of lines at the speed of your disk.

## Features

* 🚀 **High-Performance Parsing:** Powered by `simdjson` for parsing JSON at gigabytes per second.
* 🧠 **Lazy Loading Architecture:** Utilizes C++20 Coroutines (`co_yield`) to read and parse only the data currently visible on your screen.
* 🔍 **Smart Search & Regex:** Instantly filter logs using substrings or complex Regular Expressions.
* 📡 **Live Monitoring:** Real-time log tracking (similar to `tail -f`) to monitor active servers.
* 💾 **Instant Export:** Export your carefully filtered regex search results into a new JSON file with a single keystroke.
* 🧹 **Aggressive Memory Management:** Keep your RAM usage strictly under control with on-demand memory clearing.
* 📱 **Responsive UI:** Adapts to your terminal size dynamically, built with `FTXUI`.

## Technology Stack

* **C++20** (Requires a compiler with Coroutines support, e.g., MSVC 2022, GCC 10+, Clang 14+)
* **[FTXUI](https://github.com/ArthurSonzogni/FTXUI)** - For the responsive Terminal User Interface.
* **[simdjson](https://github.com/simdjson/simdjson)** - For ultra-fast JSON parsing.
* **CMake** - Build system.

## Controls & Key Bindings

| Key | Action |
| :--- | :--- |
| `↑` / `↓` | Scroll through logs |
| `Home` / `End` | Jump to the beginning or end of the loaded buffer |
| `Enter` | Apply the current search query or Regex filter |
| `F5` | **Live Mode:** Fetch the latest log entries dynamically |
| `F12` | **Clear Memory:** Clear the screen and force memory deallocation |
| `Ctrl+S` | **Export:** Save currently filtered logs to `export_hyperlog.json` |
| `Esc` | Exit application |

## Building from Source

Ensure you have CMake and a C++20 compatible compiler installed.

1. Clone the repository:
   ```bash
   git clone https://github.com/SHEST4/HyperLog.git
   cd HyperLog
   ```
2. Generate build files using CMake:
    ```bash
    cmake -B build -S .
    ```
3. Build the project:
   ```bash
    cmake --build build --config Release
   ```
## Usage

If no arguments are provided, the application will attempt to open test.json in the current working directory.
You can launch HyperLog directly from your terminal, passing the path to the log file as an argument:


# Windows
```bash
.\build\Release\HyperLog.exe path\to\your\server_logs.json
```

# Linux
```bash
./build/HyperLog /path/to/your/server_logs.json
```

# License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0). See the LICENSE file for more details.
