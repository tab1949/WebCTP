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
    MarketDataHandler(WebSocket* ws, uWS::Loop* loop, Logger* logger):
        api_(CThostFtdcMdApi::CreateFtdcMdApi("./flow")), 
        ws_(ws), loop_(loop), logger_(logger), req_id_(1) {
        api_->RegisterSpi(this);
    }

    ~MarketDataHandler() {
        api_->Release();
    }

    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void connect(const std::string& addr) {
        char front[64];
        std::memset(front, sizeof(front), 0);
        copy(front, addr);
        api_->RegisterFront(front);
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
        performed(req_id, ret);
    }
    void OnRspUserLogin(
        CThostFtdcRspUserLoginField *pRspUserLogin, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void logout(const std::string& user_id, const std::string& broker_id) {
        CThostFtdcUserLogoutField f;
        clear(&f);
        copy(f.BrokerID, broker_id);
        copy(f.UserID, user_id);
        int req = req_id_++;
        auto ret = api_->ReqUserLogout(&f, req);
        performed(req, ret);
    }
    void OnRspUserLogout(
        CThostFtdcUserLogoutField *pUserLogout, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void getTradingDay() {
        send(MDMsgCode::TRADING_DAY, 
            json {
                {"code", 0},
                {"msg", ""}
            },
            json {
                {"trading_day", api_->GetTradingDay()}
            }   
        );
    }
    
    void subscribe(const std::vector<std::string>& instruments) {
        std::vector<const char*> mem;
        for (auto i : instruments) {
            mem.emplace_back(i.c_str());
        }
        auto ret = api_->SubscribeMarketData((char**)mem.data(), mem.size());
        performed(req_id_++, ret);
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
        performed(req_id_++, ret);
    }
    void OnRspUnSubMarketData(
        CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
    
private:
    template <typename T>
    inline void clear(T* mem) noexcept {
        std::memset(mem, 0, sizeof(T));
    }
    
    template <typename T>
    inline void copy(T* mem, const string& str) noexcept {
        std::memcpy(mem, str.c_str(), str.size());
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
