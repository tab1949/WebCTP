#include <iostream>
#include <cstring>
#include <cstdlib>

#include <ThostFtdcMdApi.h>
#include <ThostFtdcTraderApi.h>

#include "Logger.hpp"

#include "Types.hpp"
#include "MessageHandler.hpp"
#include "MarketData/Handler.hpp"
#include "Trade/Handler.hpp"

int main(int argc, char** args) {
    using namespace tabxx;
    using std::string;
    using std::endl;
    
    Logger logger;

    string addr;
    string port;
    if (argc == 1) {
        addr = "localhost";
        port = "8888";
        logger.warn("Address and port not specified.");
        logger.warn("Server will listen default address and port: localhost:8888");
    }
    else if (argc == 3) {
        addr = args[1];
        port = args[2];
        logger.info("Server will listen: " + addr + ":" + port);
    }
    else {
        logger.error("Invalid argument count.");
        return 1;
    }
    uWS::App app;
    app.get("/health", [] (HttpResponse* res, HttpRequest* req) {
        res
        ->writeStatus("200 OK")
        ->writeHeader("Content-Type", "text/html")
        ->end("<html><body><h1>OK</h1></body></html>");
    })
    .ws("/market_data", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->md = std::make_unique<MarketDataHandler>(ws, uWS::Loop::get(), &logger);
            logger.info("New MarketData connection accepted.", "ws-md");
            ws->send(json {
                {"status", "ready"},
                {"server_version", "0.0.1"},
                {"ctp_api_version", CThostFtdcMdApi::GetApiVersion()}
            }.dump());
        },
        .message = [&] (WebSocket* ws, std::string_view msg, uWS::OpCode opcode) {
            if (!ws || !ws->getUserData() || !ws->getUserData()->md) {
                logger.error("nullptr", "ws-md");
                return;
            }
            json data;
            try {
                data = json::parse(msg);
            }
            catch (const std::exception& e) {
                try {
                    ws->send("Parse client message failed. \nMessage: " + std::string(msg) + "\nException: " + e.what());
                } catch (...) {}
                return;
            }
            try {
                auto res = HandleMarketDataMessage(data, *ws->getUserData()->md);
                if (!res.empty())
                    ws->send(res);
            } catch (const std::exception& e) {
                try {
                    ws->send("Error processing message: " + string(e.what()));
                } catch (...) {}
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            logger.warn("A MarketData connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code), "ws");
        },
    })
    .ws("/trade", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->trade = std::make_unique<TraderHandler>(ws, uWS::Loop::get(), &logger);
            logger.info("New Trade connection accepted.", "ws-trade");
            ws->send(json {
                {"status", "ready"},
                {"server_version", "0.0.1"},
                {"ctp_api_version", CThostFtdcTraderApi::GetApiVersion()}
            }.dump());
        },
        .message = [&] (WebSocket* ws, std::string_view msg, uWS::OpCode opcode) {
            if (!ws || !ws->getUserData() || !ws->getUserData()->trade) {
                logger.error("nullptr", "ws-trade");
                return;
            }
            json data;
            try {
                data = json::parse(msg);
            }
            catch (const std::exception& e) {
                try {
                    ws->send("Parse client message failed. \nMessage: " + std::string(msg) + "\nException: " + e.what());
                } catch (...) {
                    logger.error("Failed to send message.", "ws-trade");
                }
                return;
            }
            try {
                auto res = HandleTraderMessage(data, *ws->getUserData()->trade);
                if (!res.empty())
                    ws->send(res);
            } catch (const std::exception& e) {
                try {
                    ws->send("Error processing message: " + string(e.what()));
                } catch (...) {
                    logger.error("Failed to send message.", "ws-trade");
                }
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            logger.warn("A connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code), "ws");
        },
    })
    .listen(std::stoi(port), [&] (auto* s) {
        if (!s) {
            logger.error("Listen port " + port + "failed", "ws");
            app.close();
            return;
        }
        logger.info("Server started.", "ws");
    })
    .run();
    return 0;
}