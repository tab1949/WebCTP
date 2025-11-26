#include "Handler.hpp"

namespace tabxx {


void MarketDataHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(MDMsgCode::ERROR, pRspInfo,
        {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void MarketDataHandler::OnFrontConnected() {
    send(MDMsgCode::CONNECTED, {}, {});
}

void MarketDataHandler::OnFrontDisconnected(int reason) {
    send(MDMsgCode::DISCONNECTED, {},
        {
            {"reason", reason}
        }
    );
}

void MarketDataHandler::OnHeartBeatWarning(int time) {
    send(MDMsgCode::HEARTBEAT_TIMEOUT, {}, 
        {
            {"time", time}
        }
    );
}

void MarketDataHandler::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(MDMsgCode::LOGIN, pRspInfo,
        pRspUserLogin? json {
            {"msg", "Login success"},
            {"code", MDMsgCode::LOGIN},
            {"trading_day", pRspUserLogin->TradingDay},
            {"login_time", pRspUserLogin->LoginTime},
            {"broker_id", pRspUserLogin->BrokerID},
            {"user_id", pRspUserLogin->UserID},
            {"system_name", pRspUserLogin->SystemName},
            {"front_id", pRspUserLogin->FrontID},
            {"session_id", pRspUserLogin->SessionID},
            {"max_order_ref", pRspUserLogin->MaxOrderRef},
            {"shfe_time", pRspUserLogin->SHFETime},
            {"dce_time", pRspUserLogin->DCETime},
            {"czce_time", pRspUserLogin->CZCETime},
            {"ffex_time", pRspUserLogin->FFEXTime},
            {"ine_time", pRspUserLogin->INETime},
            {"sys_version", pRspUserLogin->SysVersion},
            {"gfex_time", pRspUserLogin->GFEXTime},
            {"login_dr_identity_id", pRspUserLogin->LoginDRIdentityID},
            {"user_dr_identity_id", pRspUserLogin->UserDRIdentityID},
            {"last_login_time", pRspUserLogin->LastLoginTime},
            {"reserve_info", pRspUserLogin->ReserveInfo},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void MarketDataHandler::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(MDMsgCode::LOGOUT, pRspInfo,
        pUserLogout? json {
            {"broker_id", pUserLogout->BrokerID},
            {"user_id", pUserLogout->UserID},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}


void MarketDataHandler::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(MDMsgCode::SUBSCRIBE, pRspInfo,
        pSpecificInstrument? json {
            {"instrument_id", pSpecificInstrument->InstrumentID},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void MarketDataHandler::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(MDMsgCode::UNSUBSCRIBE, pRspInfo,
        pSpecificInstrument? json {
            {"instrument_id", pSpecificInstrument->InstrumentID},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}


void MarketDataHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    send(MDMsgCode::MARKET_DATA, {},
        pDepthMarketData? json {
            {"trading_day", pDepthMarketData->TradingDay},
            {"instrument_id", pDepthMarketData->InstrumentID},
            {"exchange_id", pDepthMarketData->ExchangeID},
            {"exchange_inst_id", pDepthMarketData->ExchangeInstID},
            {"last_price", pDepthMarketData->LastPrice},
            {"pre_settlement_price", pDepthMarketData->PreSettlementPrice},
            {"pre_close_price", pDepthMarketData->PreClosePrice},
            {"pre_open_interest", pDepthMarketData->PreOpenInterest},
            {"open_price", pDepthMarketData->OpenPrice},
            {"highest_price", pDepthMarketData->HighestPrice},
            {"lowest_price", pDepthMarketData->LowestPrice},
            {"volume", pDepthMarketData->Volume},
            {"turnover", pDepthMarketData->Turnover},
            {"open_interest", pDepthMarketData->OpenInterest},
            {"close_price", pDepthMarketData->ClosePrice},
            {"settlement_price", pDepthMarketData->SettlementPrice},
            {"upper_limit_price", pDepthMarketData->UpperLimitPrice},
            {"lower_limit_price", pDepthMarketData->LowerLimitPrice},
            {"pre_delta", pDepthMarketData->PreDelta},
            {"curr_delta", pDepthMarketData->CurrDelta},
            {"update_time", pDepthMarketData->UpdateTime},
            {"update_millisec", pDepthMarketData->UpdateMillisec},
            {"bp1", pDepthMarketData->BidPrice1},
            {"bv1", pDepthMarketData->BidVolume1},
            {"ap1", pDepthMarketData->AskPrice1},
            {"av1", pDepthMarketData->AskVolume1},
            {"bp2", pDepthMarketData->BidPrice2},
            {"bv2", pDepthMarketData->BidVolume2},
            {"ap2", pDepthMarketData->AskPrice2},
            {"av2", pDepthMarketData->AskVolume2},
            {"bp3", pDepthMarketData->BidPrice3},
            {"bv3", pDepthMarketData->BidVolume3},
            {"ap3", pDepthMarketData->AskPrice3},
            {"av3", pDepthMarketData->AskVolume3},
            {"bp4", pDepthMarketData->BidPrice4},
            {"bv4", pDepthMarketData->BidVolume4},
            {"ap4", pDepthMarketData->AskPrice4},
            {"av4", pDepthMarketData->AskVolume4},
            {"bp5", pDepthMarketData->BidPrice5},
            {"bv5", pDepthMarketData->BidVolume5},
            {"ap5", pDepthMarketData->AskPrice5},
            {"av5", pDepthMarketData->AskVolume5},
            {"average_price", pDepthMarketData->AveragePrice},
            {"action_day", pDepthMarketData->ActionDay},
            {"banding_upper_price", pDepthMarketData->BandingUpperPrice},
            {"banding_lower_price", pDepthMarketData->BandingLowerPrice}
        }: json()
    );
}

} // namespace tabxx