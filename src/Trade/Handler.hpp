#ifndef TABXX_TRADER_HANDLER_HPP_
#define TABXX_TRADER_HANDLER_HPP_

#include <atomic>
#include <cstring>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <ThostFtdcTraderApi.h>

#include "../Logger.hpp"
#include "../Types.hpp"
#include "../Encoding.hpp"
#include "MessageCode.hpp"
#include "Flags.hpp"

namespace tabxx {
class TraderHandler final: public CThostFtdcTraderSpi {
    using string = std::string;

public:
    TraderHandler(WebSocket* ws, uWS::Loop* loop, Logger* log, const string& flow): 
        logger_(log), 
        api_(CThostFtdcTraderApi::CreateFtdcTraderApi(flow.c_str())), 
        ws_(ws), loop_(loop), req_id_(1) {
        api_->RegisterSpi(this);
    }
    
    ~TraderHandler() {
        api_->Release();
    }

    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void connect(const string& addr, const string& port);
    void OnFrontConnected() override;
    void OnFrontDisconnected(int nReason) override;

    void setBrokerID(const std::string& broker_id);
    void setInvestorID(const std::string& investor_id);

    void getTradingDay();

    void auth(const string& user_id, const string& app_id, const string& auth_code);
    void OnRspAuthenticate(
        CThostFtdcRspAuthenticateField *pRspAuthenticateField, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void login(const std::string& user_id, const std::string& password);
    void OnRspUserLogin(
        CThostFtdcRspUserLoginField *pRspUserLogin, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void logout(const std::string& user_id);
    void OnRspUserLogout(
        CThostFtdcUserLogoutField *pUserLogout, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void querySettlementInfo(
        const string& trading_day,
        const string& account_id = "", 
        const string& currency_id = "");
    void OnRspQrySettlementInfo(
        CThostFtdcSettlementInfoField *pSettlementInfo, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void confirmSettlementInfo(
        const string& confirm_date = "",
        const string& confirm_time = "",
        int settlement_id = 0,
        const string& account_id = "",
        const string& currency_id = "");
    void OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void queryTradingAccount(
        const string& account_id = "",
        const string& currency_id = "CNY",
        char biz_type = THOST_FTDC_BZTP_Future);
    void OnRspQryTradingAccount(
        CThostFtdcTradingAccountField *pTradingAccount, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void insertOrder(
        const string& instrument, const string& exchange,
        const string& ref,
        double price, Direction direction, 
        OrderOffset offset, int volume, 
        OrderPriceType price_type, TimeCondition time_condition,
        const string& memo = "",
        Hedge hedge = Hedge::SPECULATION);
    void OnRspOrderInsert(CThostFtdcInputOrderField*, CThostFtdcRspInfoField*, int, bool) override;
    void OnErrRtnOrderInsert(CThostFtdcInputOrderField*, CThostFtdcRspInfoField*) override;
    void OnRtnOrder(CThostFtdcOrderField*) override;
    void OnRtnTrade(CThostFtdcTradeField*) override;

    void queryOrder();
    void queryOrderByID(const string& sysID);
    void queryOrderByExchange(const string& ex);
    void queryOrderByRange(const string& from, const string& to);
    void OnRspQryOrder(
        CThostFtdcOrderField *pOrder, 
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    void deleteOrder(const string& exchange, const string& instrument, int delRef, const string& sysID);
    void OnRspOrderAction(CThostFtdcInputOrderActionField*, CThostFtdcRspInfoField*, int, bool) override;
    void OnErrRtnOrderAction(CThostFtdcOrderActionField*, CThostFtdcRspInfoField*) override;
    // OnRtnOrder() is defined

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
        send(TradeMsgCode::PERFORMED, {{"code", err}}, {{"req_id", req_id}});
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

    inline void send(TradeMsgCode code, const json& err, const json& info) {
        send({
            {"msg", code},
            {"err", err},
            {"info", info}
        });
    }

    inline void send(TradeMsgCode code, const CThostFtdcRspInfoField *pRspInfo, const json& info) {
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
    Logger* logger_;
    CThostFtdcTraderApi* api_;
    WebSocket* ws_;
    uWS::Loop* loop_;
    std::atomic<int> req_id_;
    string broker_id_;
    string investor_id_;

}; // class TraderHandler

} // namespace tabxx

#endif // TABXX_TRADER_HANDLER_HPP_