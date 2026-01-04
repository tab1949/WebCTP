// Basic smoke test for tabxx::Logger.
#include "../src/Logger.hpp"

#include <chrono>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using tabxx::Logger;

namespace {

std::string read_all(const std::filesystem::path& p) {
	std::ifstream in(p, std::ios::in | std::ios::binary);
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

std::size_t count_lines(const std::string& s) {
	return static_cast<std::size_t>(std::count(s.begin(), s.end(), '\n'));
}

bool wait_for_lines(const std::filesystem::path& p, std::size_t lines, std::chrono::milliseconds timeout) {
	auto deadline = std::chrono::steady_clock::now() + timeout;
	while (std::chrono::steady_clock::now() < deadline) {
		if (std::filesystem::exists(p)) {
			std::ifstream in(p);
			std::size_t count = 0;
			std::string tmp;
			while (std::getline(in, tmp)) ++count;
			if (count >= lines) return true;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return false;
}

} // namespace

int main() {
	namespace fs = std::filesystem;

	const fs::path log_path = fs::current_path() / "logger_test.log";
	std::error_code ec;
	fs::remove(log_path, ec);

	// Logger destructor blocks because the worker never exits, so leak the instance intentionally for test runtime.
	Logger* logger = new Logger(log_path.string(), "logger-test");
	logger->info("hello", "scope-a");
	logger->warn("careful");
	logger->error("boom", "scope-b");

	if (!wait_for_lines(log_path, 3, std::chrono::milliseconds(1500))) {
		std::cerr << "Timeout waiting for log writes" << std::endl;
		return 1;
	}

	const std::string basic_content = read_all(log_path);
	if (basic_content.find("[INFO ] [scope-a] hello") == std::string::npos) {
		std::cerr << "INFO line missing expected scope/text" << std::endl;
		return 1;
	}
	if (basic_content.find("[WARN ] [logger-test] careful") == std::string::npos) {
		std::cerr << "WARN line missing default scope/text" << std::endl;
		return 1;
	}
	if (basic_content.find("[ERROR] [scope-b] boom") == std::string::npos) {
		std::cerr << "ERROR line missing expected scope/text" << std::endl;
		return 1;
	}

	// Multi-threaded logging to verify concurrency safety.
	constexpr int thread_count = 4;
	constexpr int per_thread = 20;
	const std::size_t expected_total = 3 + static_cast<std::size_t>(thread_count * per_thread);

	std::vector<std::thread> workers;
	workers.reserve(thread_count);
	for (int t = 0; t < thread_count; ++t) {
		workers.emplace_back([&logger, t]() {
			for (int i = 0; i < per_thread; ++i) {
				logger->info("mt-" + std::to_string(t) + "-" + std::to_string(i), "mt");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		});
	}

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
	for (auto& th : workers) {
        if (th.joinable()) {
            th.join();
        }
    } 

    std::cout << "Deleting logger instance..." << std::endl;
    delete logger;

	if (!wait_for_lines(log_path, expected_total, std::chrono::milliseconds(1000))) {
		std::cerr << "Timeout waiting for multi-threaded log writes" << std::endl;
		return 1;
	}

	const std::string mt_content = read_all(log_path);
	if (count_lines(mt_content) < expected_total) {
		std::cerr << "Unexpected line count after multi-threaded logging" << std::endl;
		return 1;
	}
	if (mt_content.find("[INFO ] [mt] mt-0-0") == std::string::npos) {
		std::cerr << "Multi-threaded log content missing expected entry" << std::endl;
		return 1;
	}

	return 0;
}
