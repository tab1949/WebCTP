#include "WebSocketApp.hpp"

#include "Types.hpp"
#include "MessageHandler.hpp"
#include "MarketData/Handler.hpp"
#include "Trade/Handler.hpp"

namespace tabxx {

string mkmsg(const string& msg, const json& err, const json& info) {
    return json {
        {"msg", msg},
        {"err", err},
        {"info", info}
    }.dump();
}

void WebSocketApp::init() {
    app_.get("/health", [] (HttpResponse* res, HttpRequest* req) {
        res
        ->writeStatus("200 OK")
        ->writeHeader("Content-Type", "text/html; charset=utf-8")
        ->end("<html><body><h1>OK</h1></body></html>");
    })
    .ws("/market_data", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->md = std::make_unique<MarketDataHandler>(ws, uWS::Loop::get(), &logger_, flow_);
            logger_.info("New MarketData connection accepted.", "ws-md");
            ws->send(mkmsg("ready", json{}, 
                json {
                    {"server_version", "0.0.1"},
                    {"ctp_api_version", CThostFtdcMdApi::GetApiVersion()}
                }
            ));
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
                    ws->send(mkmsg("parse_error",
                        json {
                            {"msg", "Invalid JSON payload"}
                        },
                        json{}
                    ));
                } catch (...) {
                    logger_.error("Failed to send error response", "ws-md");
                }
                return;
            }
            try {
                auto res = HandleMarketDataMessage(data, *ws->getUserData()->md);
                if (!res.empty()) {
                    ws->send(mkmsg("error",
                        json {
                            {"msg", res}
                        },
                        json{}
                    ));
                }
            } catch (const std::exception& e) {
                try {
                    ws->send(mkmsg("processing_error",
                        json {
                            {"msg", "Processing error"}
                        },
                        json{}
                    ));
                } catch (...) {
                    logger_.error("Failed to send error response", "ws-md");
                }
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            // Reset handler to stop background callbacks accessing closed ws
            if (ws && ws->getUserData()) {
                ws->getUserData()->md.reset();
            }
            logger_.warn("A MarketData connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code));
        },
    })
    .ws("/trade", uWS::App::WebSocketBehavior<WSContext> {
        .open = [&] (WebSocket* ws) {
            ws->getUserData()->trade = std::make_unique<TraderHandler>(ws, uWS::Loop::get(), &logger_, flow_);
            logger_.info("New Trade connection accepted.", "ws-trade");
            ws->send(mkmsg("ready", json{},
                json {
                    {"server_version", "0.0.1"},
                    {"ctp_api_version", CThostFtdcTraderApi::GetApiVersion()}
                }
            ));
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
                    ws->send(mkmsg("parse_error",
                        json {
                            {"msg", "Invalid JSON payload"}
                        },
                        json{}
                    ));
                } catch (...) {
                    logger_.error("Failed to send error response", "ws-trade");
                }
                return;
            }
            try {
                auto res = HandleTraderMessage(data, *ws->getUserData()->trade);
                if (!res.empty()) {
                    ws->send(mkmsg("error",
                        json {
                            {"msg", res}
                        },
                        json{}
                    ));
                }
            } catch (const std::exception& e) {
                try {
                    ws->send(mkmsg("processing_error",
                        json {
                            {"msg", "Processing error"}
                        },
                        json{}
                    ));
                } catch (...) {
                    logger_.error("Failed to send error response", "ws-trade");
                }
            }
        },
        .close = [&] (WebSocket* ws, int code, std::string_view msg) {
            if (ws && ws->getUserData()) {
                if (ws->getUserData()->trade) {
                    ws->getUserData()->trade->onClosed();
                }
                ws->getUserData()->trade.reset();
            }
            logger_.warn("A Trade connection has been closed, message: " + string(msg) + ", code: " + std::to_string(code));
        },
    })
    .listen(addr_, std::stoi(port_), [&] (auto* s) {
        if (!s) {
            logger_.error("Listen failed on " + addr_ + ":" + port_);
            app_.close();
            return;
        }
        logger_.info("Server started.");
        flag_runnable_ = true;
    });
}

} // namespace tabxx