#ifndef TABXX_WEBSOCKET_APP_HPP_
#define TABXX_WEBSOCKET_APP_HPP_

#include <string>
#include <uWebSockets/App.h>

#include "Logger.hpp"

namespace tabxx {
using std::string;

class WebSocketApp {
public:
    WebSocketApp(const string& addr, const string& port, const string& flow):
        addr_(addr), port_(port), flow_(flow) {
        try {
            init();
        } catch (const std::exception& e) {
            logger_.error("Failed to initialize WebSocketApp: " + string(e.what()));
            throw;
        }
    }

    ~WebSocketApp() {
    }

    void run() {
        logger_.info("Server will listen: " + addr_ + ":" + port_);
        app_.run();
    }

private:
    void init();

private:
    Logger logger_;
    uWS::App app_;
    string flow_;
    string addr_;
    string port_;
};

} // namespace tabxx

#endif // TABXX_WEBSOCKET_APP_HPP_