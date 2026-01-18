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
#include <queue>
#include <thread>
#include <atomic>

namespace tabxx {

inline std::string operator ""_s(const char* s, std::size_t len) {
    return s;
}

class Logger {
protected:
    struct Message {
        std::chrono::system_clock::time_point time;
        int level;
        std::string scope;
        std::string text;
    };

public:
    Logger(const std::string& default_scope = "default") : 
        default_scope_(default_scope),
        wait_time_(100), 
        worker_stop_(false), 
        worker_thread_(&Logger::worker, this) {
    }

    Logger(const std::string& filename, const std::string& default_scope) : Logger(default_scope) {
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
        worker_stop_.store(true);
        if (worker_thread_.joinable()) 
            worker_thread_.join();
        if (file_)
            file_->close();
    }

    Logger& info(const std::string& s, const std::string& scope = "") {
        return this->enqueue(Message{std::chrono::system_clock::now() + std::chrono::hours(8), 0, scope, s});
    }

    Logger& warn(const std::string& s, const std::string& scope = "") {
        return this->enqueue(Message{std::chrono::system_clock::now() + std::chrono::hours(8), 1, scope, s});
    }

    Logger& error(const std::string& s, const std::string& scope = "") {
        return this->enqueue(Message{std::chrono::system_clock::now() + std::chrono::hours(8), 2, scope, s});
    }

    Logger& setDefaultScope(const std::string& scope) {
        std::lock_guard<std::mutex> lock(mutex_);
        default_scope_ = scope;
        return *this;
    }

    Logger& setWaitTime(std::chrono::milliseconds timeout) {
        std::lock_guard<std::mutex> lock(mutex_);
        wait_time_ = timeout;
        return *this;
    }
    
protected:
    Logger& enqueue(Message&& msg) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        log_queue_.push(std::move(msg));
        return *this;
    }

    void write(const Message& msg) noexcept {
        try {
            auto time_t = std::chrono::system_clock::to_time_t(msg.time);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                msg.time.time_since_epoch()) % 1000;
            
            std::tm* tm = std::gmtime(&time_t);

            const char* lv;
            const char* color_start = "";
            constexpr const char* color_reset = "\033[0m";
            switch (msg.level) {
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
            
            const std::string& use_scope = msg.scope.empty() ? default_scope_ : msg.scope;
            size_t est_size = std::strlen(time_buf) + use_scope.size() + msg.text.size() + 32;
   
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
            console_msg += msg.text;
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
                file_msg += msg.text;
                file_msg += '\n'; 
            }

            std::cout.write(console_msg.c_str(), console_msg.size());
            std::cout.flush();
            
            if (file_ && file_->is_open() && file_->good()) {
                file_->write(file_msg.c_str(), file_msg.size());
                file_->flush();
            }
        }
        catch (const std::exception& e) {
            try {
                std::cerr << "[Logger Error] Exception in write(): " << e.what() << std::endl;
            } catch (...) {
            }
        }
        catch(...) {
            try {
                std::cerr << "[Logger Error] Unknown exception in write()" << std::endl;
            } catch (...) {
            }
        }
    }

    void worker() {
        bool run = true;
        while (run) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (log_queue_.empty()) {
                lock.unlock();
                if (worker_stop_.load())
                    break;
                std::this_thread::sleep_for(this->wait_time_);
                continue;
            }
            Message msg(log_queue_.front());
            log_queue_.pop();
            if (worker_stop_.load())
                run = false;
            if (run) {
                lock.unlock();
                this->write(msg);
            }
            else { // flush remaining logs, keep lock held
                this->write(msg);
                while (!log_queue_.empty()) {
                    Message m(log_queue_.front());
                    log_queue_.pop();
                    this->write(m);
                }
                lock.unlock();
                break;
            }
        }
    }

private:
    std::shared_ptr<std::fstream> file_;
    std::mutex mutex_;
    std::string default_scope_;
    std::queue<Message> log_queue_;
    std::thread worker_thread_;
    std::atomic<bool> worker_stop_;
    std::chrono::milliseconds wait_time_;

}; // class Logger

} // namespace tabxx


#ifdef TABXX_USE_STRINGIFY
using tabxx::operator""_s;
#endif // TABXX_USE_STRINGIFY

#endif // LOG_HPP_
