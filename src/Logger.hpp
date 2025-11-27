#ifndef LOG_HPP_
#define LOG_HPP_

#include <ctime>
#include <fstream>
#include <memory>
#include <mutex>
#include <iostream>
#include <sstream>
#include <string>

namespace tabxx {

inline std::string operator ""_s(const char* s, std::size_t len) {
    return s;
}

class Logger {
public:
    Logger() {
    }

    Logger(const std::string& filename) :
        file_(new std::fstream(filename, std::ios::app)) {
    }

    ~Logger() {

    }

    Logger& info(const std::string& s, const std::string& scope = "default") {
        return this->write(s, 0, scope);
    }

    Logger& warn(const std::string& s, const std::string& scope = "default") {
        return this->write(s, 1, scope);
    }

    Logger& error(const std::string& s, const std::string& scope = "default") {
        return this->write(s, 2, scope);
    }

protected:
    Logger& write(const std::string& str, int level, const std::string& scope) noexcept {
        try {
            std::lock_guard<std::mutex> lock(mutex_);
            std::stringstream ss;
            std::time_t time;
            std::time(&time);
            std::tm* now = std::gmtime(&time);

            std::string lv;
            const char* color_start = "";
            constexpr const char* color_reset = "\033[0m";
            switch (level) {
            case 0:
                lv = "INFO ";
                color_start = "\033[32m";
                break;
            case 1:
                lv = "WARN ";
                color_start = "\033[33m";
                break;
            case 2:
                lv = "ERROR";
                color_start = "\033[31m";
                break;
            default:
                lv = "?   ";
                color_start = "\033[36m";
            }
            ss << "[UTC " << now->tm_year+1900 << '-' << now->tm_mon+1 << '-' << now->tm_mday << ' ' << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << "] [" << color_start << lv << color_reset << "] [" << scope << "] " << str << std::endl;
            std::cout.write(ss.str().c_str(), ss.str().size());
            if (file_)
                file_->write(ss.str().c_str(), ss.str().size());
        }
        catch(...) {}
        return *this;
    }

private:
    std::shared_ptr<std::fstream> file_;
    std::mutex mutex_;
}; // class Logger

} // namespace tabxx


#ifdef TABXX_USE_STRINGIFY
using tabxx::operator""_s;
#endif // TABXX_USE_STRINGIFY

#endif // LOG_HPP_
