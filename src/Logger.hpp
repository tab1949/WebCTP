#ifndef LOG_HPP_
#define LOG_HPP_

#include <ctime>
#include <cstring>
#include <chrono>
#include <fstream>
#include <memory>
#include <mutex>
#include <iostream>
#include <string>
#include <filesystem>

namespace tabxx {

inline std::string operator ""_s(const char* s, std::size_t len) {
    return s;
}

class Logger {
public:
    Logger(const std::string& default_scope = "default") : default_scope_(default_scope) {
    }

    Logger(const std::string& filename, const std::string& default_scope) : default_scope_(default_scope) {
        try {
            std::filesystem::path file_path(filename);
            std::filesystem::path dir_path = file_path.parent_path();
            
            if (!dir_path.empty() && !std::filesystem::exists(dir_path)) {
                std::filesystem::create_directories(dir_path);
            }
            
            file_ = std::make_shared<std::fstream>(filename, std::ios::app | std::ios::out);
            if (!file_->is_open() || !file_->good()) {
                std::cerr << "tabxx::Logger(): failed to open log file: " << filename << std::endl;
                file_.reset();
            }
        } catch (const std::exception& e) {
            std::cerr << "tabxx::Logger(): failed to create directory or open log file. Exception: " << e.what() << std::endl;
            try {
                file_ = std::make_shared<std::fstream>(filename, std::ios::app | std::ios::out);
                if (!file_->is_open() || !file_->good()) {
                    file_.reset();
                }
            } catch (...) {
                file_.reset();
            }
        }
    }

    ~Logger() {
        if (file_)
            file_->close();
    }

    Logger& info(const std::string& s, const std::string& scope = "") {
        return this->write(s, 0, scope);
    }

    Logger& warn(const std::string& s, const std::string& scope = "") {
        return this->write(s, 1, scope);
    }

    Logger& error(const std::string& s, const std::string& scope = "") {
        return this->write(s, 2, scope);
    }

protected:
    Logger& write(const std::string& str, int level, const std::string& scope) noexcept {
        try {
            auto time_utc8 = std::chrono::system_clock::now() + std::chrono::hours(8); // UTC+8
            auto time_t = std::chrono::system_clock::to_time_t(time_utc8);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                time_utc8.time_since_epoch()) % 1000;
            
            std::tm* tm = std::gmtime(&time_t);

            const char* lv;
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
            
            char time_buf[64];
            std::snprintf(time_buf, sizeof(time_buf), "[UTC+8 %04d-%02d-%02d %02d:%02d:%02d.%03ld]", 
                         tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
                         tm->tm_hour, tm->tm_min, tm->tm_sec, ms.count());
            
            const std::string& use_scope = scope.empty() ? default_scope_ : scope;
            size_t est_size = std::strlen(time_buf) + use_scope.size() + str.size() + 32;
   
            std::string console_msg;
            console_msg.reserve(est_size + 16);
            console_msg = time_buf;
            console_msg += " [";
            console_msg += color_start;
            console_msg += lv;
            console_msg += color_reset;
            console_msg += "] [";
            console_msg += "\033[90m";
            console_msg += use_scope;
            console_msg += color_reset;
            console_msg += "] ";
            console_msg += str;
            console_msg += '\n';
            
            std::string file_msg;
            if (file_) {
                file_msg.reserve(est_size);
                file_msg = time_buf;
                file_msg += " [";
                file_msg += lv;
                file_msg += "] [";
                file_msg += use_scope;
                file_msg += "] ";
                file_msg += str;
                file_msg += '\n'; 
            }

            std::lock_guard<std::mutex> lock(mutex_);
            std::cout.write(console_msg.c_str(), console_msg.size());
            std::cout.flush();
            
            if (file_ && file_->is_open() && file_->good()) {
                file_->write(file_msg.c_str(), file_msg.size());
                file_->flush();
            }
        }
        catch (const std::exception& e) {
            try {
                std::lock_guard<std::mutex> lock(mutex_);
                std::cerr << "[Logger Error] Exception in write(): " << e.what() << std::endl;
            } catch (...) {
            }
        }
        catch(...) {
            try {
                std::lock_guard<std::mutex> lock(mutex_);
                std::cerr << "[Logger Error] Unknown exception in write()" << std::endl;
            } catch (...) {
            }
        }
        return *this;
    }

private:
    std::shared_ptr<std::fstream> file_;
    std::mutex mutex_;
    std::string default_scope_;

}; // class Logger

} // namespace tabxx


#ifdef TABXX_USE_STRINGIFY
using tabxx::operator""_s;
#endif // TABXX_USE_STRINGIFY

#endif // LOG_HPP_
