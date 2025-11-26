#include <stdexcept>

#include "Handler.hpp"

namespace tabxx {

void TraderHandler::connect(const string& addr, const string& port) {
    char a[64];
    clear(a);
    string str = string("tcp://" + addr + ":" + port);
    copy(a, str);
    api_->RegisterFront(a);
    api_->Init();
    performed(0, 0);
}

void TraderHandler::setBrokerID(const std::string& broker_id) {
    int req = req_id_++;
    if (broker_id.size() >= sizeof(TThostFtdcBrokerIDType)) {
        performed(req, -1);
        return;
    }
    broker_id_ = broker_id;
    performed(req, 0);
}

void TraderHandler::setInvestorID(const std::string& investor_id) {
    int req = req_id_++;
    if (investor_id.size() >= sizeof(TThostFtdcInvestorIDType)) {
        performed(req, -1);
        return;
    }
    investor_id_ = investor_id;
    performed(req, 0);
}

void TraderHandler::getTradingDay() {
    send(TradeMsgCode::TRADING_DAY, {}, {
        {"trading_day", api_->GetTradingDay()}
    });
}

void TraderHandler::auth(const string& user_id, const string& app_id, const string& auth_code) {
    CThostFtdcReqAuthenticateField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.AppID, app_id);
    copy(f.AuthCode, auth_code);
    copy(f.UserID, user_id);
    int req_id = req_id_++;
    int err = api_->ReqAuthenticate(&f, req_id);
    performed(req_id, err);
}

void TraderHandler::login(const std::string& user_id, const std::string& password) {
    CThostFtdcReqUserLoginField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.UserID, user_id);
    copy(f.Password, password);
    int req_id = req_id_ ++;
    auto ret = api_->ReqUserLogin(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::logout(const std::string& user_id) {
    CThostFtdcUserLogoutField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.UserID, user_id);
    int req_id = req_id_++;
    auto ret = api_->ReqUserLogout(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::querySettlementInfo(
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
    performed(req_id, ret);
}

void TraderHandler::confirmSettlementInfo(
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
    performed(req_id, ret);
}

void TraderHandler::queryTradingAccount(
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
    performed(req_id, ret);
}

void TraderHandler::insertOrder(
    const string& instrument, const string& exchange,
    const string& ref,
    double price, Direction direction, 
    OrderOffset offset, int volume, 
    OrderPriceType price_type, TimeCondition time_condition,
    const string& memo,
    Hedge hedge) {
    string report = "RECEIVED: " + instrument + " in " + exchange + " with ref '" + ref + "'";
    CThostFtdcInputOrderField f;
    clear(&f);
    switch (direction) {
    case Direction::BUY:
        f.Direction = THOST_FTDC_D_Buy;
        report += " BUY";
        break;
    case Direction::SELL:
        f.Direction = THOST_FTDC_D_Sell;
        report += " SELL";
        break;
    default:
        throw std::runtime_error("TraderHandler::insertOrder(): Unknown Buy/Sell Direction");
        break;
    }
    switch (offset) {
    case OrderOffset::OPEN:
        f.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
        report += " OPEN";
        break;
    case OrderOffset::CLOSE:
        f.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
        report += " CLOSE";
        break;
    case OrderOffset::CLOSE_TODAY:
        f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
        report += " CLOSE_TODAY";
        break;
    case OrderOffset::CLOSE_YESTERDAY:
        f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
        report += " CLOSE_YESTERDAY";
        break;
    default:
        throw std::runtime_error("TraderHandler::insertOrder(): Unsupported Open/Close operation");
    }
    report += " " + std::to_string(volume);
    switch (price_type) {
    case OrderPriceType::LIMITED:
        f.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
        report += " LIMITED";
        break;
    case OrderPriceType::MARKET:
        f.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
        report += " MARKET";
        break;
    case OrderPriceType::LAST:
        f.OrderPriceType = THOST_FTDC_OPT_LastPrice;
        report += " LAST";
        break;
    default:
        throw std::runtime_error("TraderHandler::insertOrder(): Unsupported Price Type");
    }
    report += " at " + std::to_string(price);
    switch (hedge) {
    case Hedge::SPECULATION:
        f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
        break;
    default:
        throw std::runtime_error("TraderHandler::insertOrder(): Unsupported Hedge Flag");
    }
    switch (time_condition) {
    case TimeCondition::IMMEDIATE:
        f.TimeCondition = THOST_FTDC_TC_IOC;
        report += " IMMEDIATELY";
        break;
    case TimeCondition::ONE_DAY:
        f.TimeCondition = THOST_FTDC_TC_GFD;
        report += " ONE_DAY";
        break;
    default:
        throw std::runtime_error("TraderHandler::insertOrder(): Unsupported Time Condition");
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
    int req_id = req_id_++;
    f.RequestID = req_id;
    auto ret = api_->ReqOrderInsert(&f, req_id);
    report += " returned " + std::to_string(ret);
    performed(req_id, ret, report);
}
    
void TraderHandler::queryOrder() {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::queryOrderByID(const string& sysID) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.OrderSysID, sysID);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::queryOrderByExchange(const string& ex) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.ExchangeID, ex);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::queryOrderByRange(const string& from, const string& to) {
    CThostFtdcQryOrderField f;
    clear(&f);
    copy(f.BrokerID, broker_id_);
    copy(f.InvestorID, investor_id_);
    copy(f.InsertTimeStart, from);
    copy(f.InsertTimeEnd, to);
    int req_id = req_id_++;
    auto ret = api_->ReqQryOrder(&f, req_id);
    performed(req_id, ret);
}

void TraderHandler::deleteOrder(const string& exchange, const string& instrument, int delRef, const string& sysID) {
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
    performed(req_id, ret);
}

void TraderHandler::queryInstrument(const string& exchange, const string& instrument, const string& exchange_inst_id, const string& product_id) {
    CThostFtdcQryInstrumentField f;
    clear(&f);
    copy(f.ExchangeID, exchange);
    copy(f.InstrumentID, instrument);
    copy(f.ExchangeInstID, exchange_inst_id);
    copy(f.ProductID, product_id);
    int req_id = req_id_++;
    auto ret = api_->ReqQryInstrument(&f, req_id);
    performed(req_id, ret);
}

} // namespace tabxx