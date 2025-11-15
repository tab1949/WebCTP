#include "WebSocketApp.hpp"

#include "Types.hpp"
#include "MessageHandler.hpp"
#include "MarketData/Handler.hpp"
#include "Trade/Handler.hpp"

namespace tabxx {

void WebSocketApp::init() {
    app_.get("/health", [] (HttpResponse* res, HttpRequest* req) {
        res
        ->writeStatus("200 OK")
        ->writeHeader("Content-Type", "text/html")
        ->end("<html><body><h1>OK</h1></body></html>");
    })
    .ws("/market_data", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->md = std::make_unique<MarketDataHandler>(ws, uWS::Loop::get(), &logger_, flow_);
            logger_.info("New MarketData connection accepted.", "ws-md");
            ws->send(json {
                {"status", "ready"},
                {"server_version", "0.0.1"},
                {"ctp_api_version", CThostFtdcMdApi::GetApiVersion()}
            }.dump());
        },
        .message = [&] (WebSocket* ws, std::string_view msg, uWS::OpCode opcode) {
            if (!ws || !ws->getUserData() || !ws->getUserData()->md) {
                logger_.error("nullptr", "ws-md");
                return;
            }
            json data;
            try {
                data = json::parse(msg);
            }
            catch (const std::exception& e) {
                try {
                    ws->send(json {
                        {"msg", "Parse client message failed. \nMessage: " + string(msg) + "\nException: " + e.what()}
                    }.dump());
                } catch (...) {
                    logger_.error((string)"Failed to send message. Unsent: " + e.what(), "ws-md");
                }
                return;
            }
            try {
                auto res = HandleMarketDataMessage(data, *ws->getUserData()->md);
                if (!res.empty())
                    ws->send(json{{"msg", res}}.dump());
            } catch (const std::exception& e) {
                try {
                    ws->send(json {
                        {"msg", "Error processing message: " + string(e.what())}
                    }.dump());
                } catch (...) {
                    logger_.error((string)"Failed to send message. Unsent: " + e.what(), "ws-md");
                }
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            logger_.warn("A MarketData connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code), "ws");
        },
    })
    .ws("/trade", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->trade = std::make_unique<TraderHandler>(ws, uWS::Loop::get(), &logger_, flow_);
            logger_.info("New Trade connection accepted.", "ws-trade");
            ws->send(json {
                {"status", "ready"},
                {"server_version", "0.0.1"},
                {"ctp_api_version", CThostFtdcTraderApi::GetApiVersion()}
            }.dump());
        },
        .message = [&] (WebSocket* ws, std::string_view msg, uWS::OpCode opcode) {
            if (!ws || !ws->getUserData() || !ws->getUserData()->trade) {
                logger_.error("nullptr", "ws-trade");
                return;
            }
            json data;
            try {
                data = json::parse(msg);
            }
            catch (const std::exception& e) {
                try {
                    ws->send(json {
                        {"msg", "Parse client message failed. \nMessage: " + std::string(msg) + "\nException: " + e.what()}
                    }.dump());
                } catch (...) {
                    logger_.error((string)"Failed to send message. Unsent: " + e.what(), "ws-trade");
                }
                return;
            }
            try {
                auto res = HandleTraderMessage(data, *ws->getUserData()->trade);
                if (!res.empty())
                    ws->send(json {
                        {"msg", res}
                    }.dump());
            } catch (const std::exception& e) {
                try {
                    ws->send(json {
                        {"msg", "Error processing message: " + string(e.what())}
                    }.dump());
                } catch (...) {
                    logger_.error((string)"Failed to send message. Unsent: " + e.what(), "ws-trade");
                }
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            logger_.warn("A connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code), "ws");
        },
    })
    .listen(std::stoi(port_), [&] (auto* s) {
        if (!s) {
            logger_.error("Listen port " + port_ + "failed", "ws");
            app_.close();
            return;
        }
        logger_.info("Server started.", "ws");
    });
}

} // namespace tabxx