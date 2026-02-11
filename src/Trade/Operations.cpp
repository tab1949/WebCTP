#include <stdexcept>

#include "Handler.hpp"

namespace tabxx {

void TraderHandler::connect(const string& op_ref, const string& addr, const string& port) {
    string str = "tcp://" + addr + ":" + port;
    info("Client attempting to connect to Trade front: "_s + str);
    api_->RegisterFront(str.data());
    api_->Init();
    performed(op_ref, 0, 0);
}

void TraderHandler::setBrokerID(const string& op_ref, const std::string& broker_id) {
    int req = req_id_++;
    if (broker_id.size() >= sizeof(TThostFtdcBrokerIDType)) {
        performed(op_ref, req, -1, "broker_id is too long");
        return;
    }
    broker_id_ = broker_id;
    info("Client set BrokerID to: "_s + broker_id);
    performed(op_ref, req, 0);
}

void TraderHandler::setInvestorID(const string& op_ref, const std::string& investor_id) {
    int req = req_id_++;
    if (investor_id.size() >= sizeof(TThostFtdcInvestorIDType)) {
        performed(op_ref, req, -1, "investor_id is too long");
        return;
    }
    investor_id_ = investor_id;
    info("Client set InvestorID to: "_s + investor_id);
    performed(op_ref, req, 0);
}

void TraderHandler::getTradingDay(const string& op_ref) {
    auto trading_day = api_->GetTradingDay();
    info("Client query TradingDay. Trading Day: "_s + std::string(trading_day));
    send(TradeMsgCode::TRADING_DAY, {}, {
        {"trading_day", trading_day},
        {"op_ref", op_ref}
    });
}

void TraderHandler::auth(const string& op_ref, const string& user_id, const string& app_id, const string& auth_code) {
    CThostFtdcReqAuthenticateField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.AppID, app_id);
    copy(f.AuthCode, auth_code);
    copy(f.UserID, user_id);
    int req_id = req_id_++;
    int err = api_->ReqAuthenticate(&f, req_id);
    info("Client sent authentication request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(err));
    performed(op_ref, req_id, err);
}

void TraderHandler::login(const string& op_ref, const std::string& user_id, const std::string& password) {
    CThostFtdcReqUserLoginField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.UserID, user_id);
    copy(f.Password, password);
    int req_id = req_id_ ++;
    auto ret = api_->ReqUserLogin(&f, req_id);
    info("Client sent login request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::logout(const string& op_ref, const std::string& user_id) {
    CThostFtdcUserLogoutField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.UserID, user_id);
    int req_id = req_id_++;
    auto ret = api_->ReqUserLogout(&f, req_id);
    info("Client sent logout request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::querySettlementInfo(
    const string& op_ref,
    const string& trading_day,
    const string& account_id, 
    const string& currency_id) {
    CThostFtdcQrySettlementInfoField f;
    clear(&f); 
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.AccountID, account_id);
    copy(f.CurrencyID, currency_id);
    copy(f.TradingDay, trading_day);
    int req_id = req_id_++;
    auto ret = api_->ReqQrySettlementInfo(&f, req_id);
    info("Client sent settlement info query request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::confirmSettlementInfo(
    const string& op_ref,
    const string& confirm_date,
    const string& confirm_time,
    int settlement_id,
    const string& account_id,
    const string& currency_id) {
    CThostFtdcSettlementInfoConfirmField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.AccountID, account_id);
    copy(f.ConfirmDate, confirm_date);
    copy(f.ConfirmTime, confirm_time);
    copy(f.CurrencyID, currency_id);
    f.SettlementID = settlement_id;
    int req_id = req_id_ ++;
    auto ret = api_->ReqSettlementInfoConfirm(&f, req_id);
    info("Client sent settlement info confirm request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::queryTradingAccount(
    const string& op_ref,
    const string& account_id,
    const string& currency_id,
    char biz_type) {
    CThostFtdcQryTradingAccountField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.AccountID, account_id);
    copy(f.CurrencyID, currency_id);
    f.BizType = biz_type;
    int req_id = req_id_++;
    auto ret = api_->ReqQryTradingAccount(&f, req_id);
    info("Client sent trading account query request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::insertOrder(
    const string& op_ref,
    const string& instrument, 
    const string& exchange,
    const string& ref,
    double price, 
    Direction direction, 
    OrderOffset offset, 
    int volume, 
    OrderPriceType price_type, 
    TimeCondition time_condition,
    const string& memo,
    Hedge hedge) {
    CThostFtdcInputOrderField f;
    clear(&f);
    try {
        f.TimeCondition = GetTimeCondition(time_condition);
        f.CombHedgeFlag[0] = GetHedge(hedge);
        f.OrderPriceType = GetOrderPriceType(price_type);
        f.CombOffsetFlag[0] = GetOrderOperation(offset);
        f.Direction = GetDirection(direction);
    }
    catch (const std::exception& e) {
        error("Exception caught in TraderHandler::insertOrder(): "_s + e.what());
        send(TradeMsgCode::ERROR_UNKNOWN_VALUE, {}, {
            {"info", "Exception caught in TraderHandler::insertOrder(): "_s + e.what()}
        });
        return;
    }
    f.LimitPrice = price;
    f.StopPrice = 0;
    f.VolumeTotalOriginal = volume;
    f.VolumeCondition = THOST_FTDC_VC_AV;
    f.ContingentCondition = THOST_FTDC_CC_Immediately;
    f.IsAutoSuspend = 0;
    f.IsSwapOrder = 0;
    f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    copy(f.ExchangeID, exchange);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.InstrumentID, instrument);
    copy(f.OrderRef, ref);
    copy(f.OrderMemo, memo);
    string report = "RECEIVED: " + MakeOrderBrief(f.OrderRef, f.InstrumentID, f.ExchangeID, volume, price, direction, offset, price_type, hedge, time_condition);
    int req_id = req_id_++;
    f.RequestID = req_id;
    auto ret = api_->ReqOrderInsert(&f, req_id);
    info("Client sent order insert request. ReqID: "_s + std::to_string(req_id) + "; Details: " + report);
    performed(op_ref, req_id, ret, report);
}
    
void TraderHandler::queryOrder(const string& op_ref) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    info("Client sent order query request. ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::queryOrderByID(const string& op_ref, const string& sysID) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.OrderSysID, sysID);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    info("Client sent order query by ID request. OrderSysID: " + sysID + "; ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::queryOrderByExchange(const string& op_ref, const string& ex) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.ExchangeID, ex);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    info("Client sent order query by Exchange request. ExchangeID: " + ex + "; ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::queryOrderByRange(const string& op_ref, const string& from, const string& to) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.InsertTimeStart, from);
    copy(f.InsertTimeEnd, to);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    info("Client sent order query by Range request. From: " + from + "; To: " + to + "; ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::deleteOrder(const string& op_ref, const string& exchange, const string& instrument, int delRef, const string& sysID) {
    CThostFtdcInputOrderActionField f;
    int req_id = req_id_++;
    clear(&f);
    f.ActionFlag = THOST_FTDC_AF_Delete;
    copy(f.BrokerID, this->broker_id_);
    copy(f.InvestorID, this->investor_id_);
    copy(f.UserID, this->investor_id_);
    copy(f.ExchangeID, exchange);
    copy(f.InstrumentID, instrument);
    f.OrderActionRef = delRef;
    copy(f.OrderSysID, sysID);
    f.RequestID = req_id;
    auto ret = api_->ReqOrderAction(&f, req_id);
    info("Client sent order delete request. OrderActionRef: " + delRef + "; ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

void TraderHandler::queryInstrument(const string& op_ref, const string& exchange, const string& instrument, const string& exchange_inst_id, const string& product_id) {
    CThostFtdcQryInstrumentField f;
    clear(&f);
    copy(f.ExchangeID, exchange);
    copy(f.InstrumentID, instrument);
    copy(f.ExchangeInstID, exchange_inst_id);
    copy(f.ProductID, product_id);
    int req_id = req_id_++;
    auto ret = api_->ReqQryInstrument(&f, req_id);
    info("Client sent instrument query request. Exchange: "_s + (exchange.empty()? "ALL": exchange) + "; Instrument: " + (instrument.empty()? "ALL": instrument) + "; ReqID: "_s + std::to_string(req_id) + "; Return: " + std::to_string(ret));
    performed(op_ref, req_id, ret);
}

} // namespace tabxx