#ifndef TABXX_MARKET_DATA_HANDLER_HPP_
#define TABXX_MARKET_DATA_HANDLER_HPP_

#include <atomic>
#include <cstring>
#include <string>
#include <vector>

#include <uWebSockets/App.h>
#include <ThostFtdcMdApi.h>
#include <json.hpp>

#include "MessageCode.hpp"
#include "../Types.hpp"
#include "../Logger.hpp"
#include "../Encoding.hpp"

namespace tabxx {
class MarketDataHandler final: public CThostFtdcMdSpi {
    using string = std::string;
public:
    MarketDataHandler(WebSocket* ws, uWS::Loop* loop, Logger* logger, const string& flow):
        api_(CThostFtdcMdApi::CreateFtdcMdApi(flow.c_str())), 
        ws_(ws), loop_(loop), logger_(logger), req_id_(1) {
        api_->RegisterSpi(this);
    }

    ~MarketDataHandler() {
        api_->Release();
        info("Market Data API Released");
    }

    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void connect(const std::string& addr, const std::string& port) {
        string front = "tcp://" + addr + ":" + port;
        info("Client attempting to connect to Market Data front: "_s + front);
        api_->RegisterFront(front.data());
        api_->Init();
        performed(0, 0);
    }
    void OnFrontConnected() override;
    void OnFrontDisconnected(int reason) override;
    void OnHeartBeatWarning(int time) override;

    void login(const std::string& broker_id, const std::string& user_id, const std::string& password) {
        CThostFtdcReqUserLoginField f;
        clear(&f);
        copy(f.BrokerID, broker_id);
        copy(f.UserID, user_id);
        copy(f.Password, password);
        int req_id = req_id_++;
        auto ret = api_->ReqUserLogin(&f, req_id);
        info("Sent login request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
        performed(req_id, ret);
    }
    void OnRspUserLogin(
        CThostFtdcRspUserLoginField *pRspUserLogin, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    // Logout is not supported by CTP
    
    void OnRspUserLogout(
        CThostFtdcUserLogoutField *pUserLogout, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void getTradingDay() {
        auto trading_day = api_->GetTradingDay();
        info("Client query Trading Day. Trading Day: "_s + std::string(trading_day));
        send(MDMsgCode::TRADING_DAY, 
            json {
                {"code", 0},
                {"msg", ""}
            },
            json {
                {"trading_day", trading_day}
            }   
        );
    }
    
    void subscribe(const std::vector<std::string>& instruments) {
        std::vector<const char*> mem;
        for (auto i : instruments) {
            mem.emplace_back(i.c_str());
        }
        auto ret = api_->SubscribeMarketData((char**)mem.data(), mem.size());
        auto req = req_id_++;
        info("Client subscribed to Market Data for "_s + std::to_string(instruments.size()) + " instruments. ReqID: " + std::to_string(req) + "; Return: "_s + std::to_string(ret));
        performed(req, ret);
    }
    void OnRspSubMarketData(
        CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
    
    void unsubscribe(const std::vector<std::string>& instruments) {
        std::vector<const char*> mem;
        for (auto i : instruments) {
            mem.emplace_back(i.c_str());
        }
        auto ret = api_->UnSubscribeMarketData((char**)mem.data(), mem.size());
        auto req = req_id_++;
        info("Client unsubscribed from Market Data for "_s + std::to_string(instruments.size()) + " instruments. ReqID: " + std::to_string(req) + "; Return: "_s + std::to_string(ret));
        performed(req, ret);
    }
    void OnRspUnSubMarketData(
        CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    
private:
    template <typename T>
    inline void clear(T* mem) noexcept {
        std::memset(mem, 0, sizeof(T));
    }
    
    void clear(char*) = delete;
    
    template <size_t N>
    inline void copy(char (&mem)[N], const string& str) noexcept {
        if (N == 0) return;
        const size_t to_copy = std::min(str.size(), N - 1);
        std::memcpy(mem, str.c_str(), to_copy);
        mem[to_copy] = '\0';
    }

    inline void info(const string& s) {
        if (logger_) {
            logger_->info(s, "market-data");
        }
    }

    inline void warn(const string& s) {
        if (logger_) {
            logger_->warn(s, "market-data");
        }
    }

    inline void error(const string& s) {
        if (logger_) {
            logger_->error(s, "market-data");
        }
    }

    inline void performed(int req_id, int err) {
        send(MDMsgCode::PERFORMED, {{"code", err}}, {{"req_id", req_id}});
    }

    inline void send(json&& data) {
        if (ws_) {
            try {
                loop_->defer([d=std::move(data), ws=ws_] () {
                    ws->send(d.dump());
                });
            } catch (const std::exception& e) {
                logger_->error("tabxx::TraderHandler::send(): Exception caught. what(): "_s + e.what());
            }
        }
    }

    inline void send(MDMsgCode code, const json& err, const json& info) {
        send({
            {"msg", code},
            {"err", err},
            {"info", info}
        });
    }

    inline void send(MDMsgCode code, const CThostFtdcRspInfoField *pRspInfo, const json& info) {
        send({
            {"msg", code},
            {"err", pRspInfo? json {
                {"code", pRspInfo->ErrorID},
                {"msg", u8(pRspInfo->ErrorMsg)}
            }: json()},
            {"info", info}
        });
    }

private:
    CThostFtdcMdApi* api_;
    WebSocket* ws_;
    uWS::Loop* loop_;
    Logger* logger_;
    std::atomic<int> req_id_;

}; // class MarketDataHandler

} // namespace tabxx

#endif // TABXX_MARKET_DATA_HANDLER_HPP_
