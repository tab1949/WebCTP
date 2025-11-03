#include "Handler.hpp"

namespace tabxx {

void TraderHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::ERROR,
    {
        {"code", pRspInfo->ErrorID},
        {"msg", u8(pRspInfo->ErrorMsg)}
    }, 
    {
        {"req_id", nRequestID},
        {"is_last", bIsLast}
    });
}

void TraderHandler::OnFrontConnected()  {
    send(TradeMsgCode::CONNECTED, {}, {});
}

void TraderHandler::OnFrontDisconnected(int nReason) {
    string reason;
    switch (nReason) {
    case 0x1001:
        reason = "Network reading failed";
        break;
    case 0x1002:
        reason = "Network writing failed";
        break;
    case 0x2001:
        reason = "Heartbeat timeout";
        break;
    case 0x2002:
        reason = "Sending heartbeat failed";
        break;
    case 0x2003:
        reason = "Invalid response received";
        break;
    default:
        break;
    }
    send(TradeMsgCode::DISCONNECTED, 
        {{"code", nReason}, {"msg", reason}},
        {}
    );
}

void TraderHandler::OnRspAuthenticate(
    CThostFtdcRspAuthenticateField *pRspAuthenticateField, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)  {
    send(TradeMsgCode::AUTHENTICATE,
        {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        },
        {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"app_id", pRspAuthenticateField->AppID},
            {"app_type", pRspAuthenticateField->AppType},
            {"broker_id", pRspAuthenticateField->BrokerID},
            {"user_id", pRspAuthenticateField->UserID},
            {"user_product_info", u8(pRspAuthenticateField->UserProductInfo)}
        }
    );
}

void TraderHandler::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)  {
    send(TradeMsgCode::LOGIN,
        {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        },
        {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
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
            {"gfex_time", pRspUserLogin->GFEXTime},
            {"sys_version", pRspUserLogin->SysVersion},
            {"login_dr_identity_id", pRspUserLogin->LoginDRIdentityID},
            {"user_dr_identity_id", pRspUserLogin->UserDRIdentityID},
            {"last_login_time", pRspUserLogin->LastLoginTime},
            // {"", pRspUserLogin->ReserveInfo}
        }
    );
}

void TraderHandler::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast) {
    send(TradeMsgCode::LOGOUT,
        {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        },
        {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"broker_id", pUserLogout->BrokerID},
            {"user_id", pUserLogout->UserID}
        }
    );
}

void TraderHandler::OnRspQrySettlementInfo(
    CThostFtdcSettlementInfoField *pSettlementInfo, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::SETTLEMENT_INFO,
        pRspInfo? json {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        }: json(),
        pSettlementInfo? json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"trading_day", pSettlementInfo->TradingDay},
            {"settlement_id", pSettlementInfo->SettlementID},
            {"broker_id", pSettlementInfo->BrokerID},
            {"investor_id", pSettlementInfo->InvestorID},
            {"sequence_no", pSettlementInfo->SequenceNo},
            {"content", u8(pSettlementInfo->Content)},
            {"account_id", pSettlementInfo->AccountID},
            {"currency_id", pSettlementInfo->CurrencyID}
        }: json()
    );
}

void TraderHandler::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::SETTLEMENT_INFO_CONFIRM,
        pRspInfo? json {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        }: json(),
        pSettlementInfoConfirm? json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"broker_id", pSettlementInfoConfirm->BrokerID},
            {"investor_id", pSettlementInfoConfirm->InvestorID},
            {"confirm_date", u8(pSettlementInfoConfirm->ConfirmDate)},
            {"confirm_time", u8(pSettlementInfoConfirm->ConfirmTime)},
            {"settlement_id", pSettlementInfoConfirm->SettlementID},
            {"account_id", pSettlementInfoConfirm->AccountID},
            {"currency_id", pSettlementInfoConfirm->CurrencyID}
        }: json()
    );
}

void TraderHandler::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::TRADING_ACCOUNT,
        pRspInfo? json {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        }: json(),
        pTradingAccount? json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"broker_id", pTradingAccount->BrokerID},
            {"account_id", pTradingAccount->AccountID},
            {"pre_mortgage", pTradingAccount->PreMortgage},
            {"pre_credit", pTradingAccount->PreCredit},
            {"pre_deposit", pTradingAccount->PreDeposit},
            {"pre_balance", pTradingAccount->PreBalance},
            {"pre_margin", pTradingAccount->PreMargin},
            {"interest_base", pTradingAccount->InterestBase},
            {"interest", pTradingAccount->Interest},
            {"deposit", pTradingAccount->Deposit},
            {"withdraw", pTradingAccount->Withdraw},
            {"frozen_margin", pTradingAccount->FrozenMargin},
            {"frozen_cash", pTradingAccount->FrozenCash},
            {"frozen_commission", pTradingAccount->FrozenCommission},
            {"current_margin", pTradingAccount->CurrMargin},
            {"cash_in", pTradingAccount->CashIn},
            {"commission", pTradingAccount->Commission},
            {"close_profit", pTradingAccount->CloseProfit},
            {"position_profit", pTradingAccount->PositionProfit},
            {"available", pTradingAccount->Available},
            {"withdraw_quota", pTradingAccount->WithdrawQuota},
            {"reserve", pTradingAccount->Reserve},
            {"trading_day", pTradingAccount->TradingDay},
            {"settlement_id", pTradingAccount->SettlementID},
            {"credit", pTradingAccount->Credit},
            {"mortgage", pTradingAccount->Mortgage},
            {"exchange_margin", pTradingAccount->ExchangeMargin},
            {"delivery_margin", pTradingAccount->DeliveryMargin},
            {"exchange_delivery_margin", pTradingAccount->ExchangeDeliveryMargin},
            {"reserve_balance", pTradingAccount->ReserveBalance},
            {"currency_id", pTradingAccount->CurrencyID},
            {"pre_fund_mortgage_in", pTradingAccount->PreFundMortgageIn},
            {"pre_fund_mortgage_out", pTradingAccount->PreFundMortgageOut},
            {"fund_mortgage_in", pTradingAccount->FundMortgageIn},
            {"fund_mortgage_out", pTradingAccount->FundMortgageOut},
            {"fund_mortgage_available", pTradingAccount->FundMortgageAvailable},
            {"mortgageable_fund", pTradingAccount->MortgageableFund},
            {"spec_product_margin", pTradingAccount->SpecProductMargin},
            {"spec_product_frozen_margin", pTradingAccount->SpecProductFrozenMargin},
            {"spec_product_commission", pTradingAccount->SpecProductCommission},
            {"spec_product_frozen_commission", pTradingAccount->SpecProductFrozenCommission},
            {"spec_product_position_profit", pTradingAccount->SpecProductPositionProfit},
            {"spec_product_close_profit", pTradingAccount->SpecProductCloseProfit},
            {"spec_product_position_profit_by_alg", pTradingAccount->SpecProductPositionProfitByAlg},
            {"spec_product_exchange_margin", pTradingAccount->SpecProductExchangeMargin},
            {"biz_type", pTradingAccount->BizType},
            {"frozen_swap", pTradingAccount->FrozenSwap},
            {"remain_swap", pTradingAccount->RemainSwap},
            {"option_value", pTradingAccount->OptionValue}
        }: json()
    );
}

void TraderHandler::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::ORDER_INSERT_ERROR,
        {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        },
        {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void TraderHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
    send(TradeMsgCode::ORDER_INSERT_ERROR,
        {
            {"code", pRspInfo->ErrorID},
            {"msg", u8(pRspInfo->ErrorMsg)}
        },
        {
            {"req_id", pInputOrder->RequestID}
        }
    );
}

void TraderHandler::OnRtnOrder(CThostFtdcOrderField *pOrder) {
    OrderSubmitStatus submitStatus;
    try {
        submitStatus = GetOrderSubmitStatus(pOrder->OrderSubmitStatus);
    }
    catch (...) {
        logger_->error("Unknown OrderSubmitStatus: "_s + pOrder->OrderStatus, "tabxx::TraderHandler::OnRtnOrder()");
        send(TradeMsgCode::ERROR_UNKNOWN_VALUE,
            {}, {{"info", "tabxx::TraderHandler::OnRtnOrder(): Unknown OrderSubmitStatus: "_s + pOrder->OrderStatus}});
    }
    OrderStatus status;
    try {
        status = GetOrderStatus(pOrder->OrderStatus);
    }
    catch (...) {
        logger_->error("Unknown OrderStatus: "_s + pOrder->OrderStatus, "tabxx::TraderHandler::OnRtnOrder()");
        send(TradeMsgCode::ERROR_UNKNOWN_VALUE,
            {}, {{"info", "tabxx::TraderHandler::OnRtnOrder(): Unknown OrderStatus: "_s + pOrder->OrderStatus}});
    }
    send(TradeMsgCode::ORDER_INSERTED,
        {
            {"code", 0},
            {"msg", ""}
        },
        {
            {"broker_id", pOrder->BrokerID},
            {"investor_id", pOrder->InvestorID},
            {"user_id", pOrder->UserID},
            {"exchange_id", pOrder->ExchangeID},
            {"req_id", pOrder->RequestID},
            {"ref", pOrder->OrderRef},
            {"order_local_id", pOrder->OrderLocalID},
            {"order_sys_id", pOrder->OrderSysID},
            {"sequence_no", pOrder->SequenceNo},
            {"instrument_id", pOrder->InstrumentID},
            {"insert_date", pOrder->InsertDate},
            {"insert_time", pOrder->InsertTime},
            {"active_time", pOrder->ActiveTime},
            {"suspend_time", pOrder->SuspendTime},
            {"update_time", pOrder->UpdateTime},
            {"cancel_time", pOrder->CancelTime},
            {"order_submit_status", submitStatus},
            {"order_status", status},
            {"volume_traded", pOrder->VolumeTraded},
            {"volume_total", pOrder->VolumeTotal},
            {"status_msg", u8(pOrder->StatusMsg)}
        }
    );
}

void TraderHandler::OnRtnTrade(CThostFtdcTradeField *pTrade) {
    send(TradeMsgCode::ORDER_TRADED,
        {
            {"code", 0},
            {"msg", ""}
        },
        {
            {"broker_id", pTrade->BrokerID},
            {"investor_id", pTrade->InvestorID},
            {"user_id", pTrade->UserID},
            {"ref", pTrade->OrderRef},
            {"exchange_id", pTrade->ExchangeID},
            {"trade_id", pTrade->TradeID},
            {"order_sys_id", pTrade->OrderSysID},
            {"order_local_id", pTrade->OrderLocalID},
            {"broker_order_seq", pTrade->BrokerOrderSeq},
            {"settlement_id", pTrade->SettlementID},
            {"volume", pTrade->Volume},
        }
    );

}

void TraderHandler::OnRspQryOrder(
    CThostFtdcOrderField *pOrder, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    OrderSubmitStatus submitStatus;
    try {
        submitStatus = GetOrderSubmitStatus(pOrder->OrderSubmitStatus);
    }
    catch (...) {
        logger_->error("Unknown OrderSubmitStatus: "_s + pOrder->OrderStatus, "tabxx::TraderHandler::OnRspQryOrder()");
        send(TradeMsgCode::ERROR_UNKNOWN_VALUE,
            {}, {{"info", "tabxx::TraderHandler::OnRspQryOrder(): Unknown OrderSubmitStatus: "_s + pOrder->OrderStatus}});
    }
    OrderStatus status;
    try {
        status = GetOrderStatus(pOrder->OrderStatus);
    }
    catch (...) {
        logger_->error("Unknown OrderStatus: "_s + pOrder->OrderStatus, "tabxx::TraderHandler::OnRspQryOrder()");
        send(TradeMsgCode::ERROR_UNKNOWN_VALUE,
            {}, {{"info", "tabxx::TraderHandler::OnRspQryOrder(): Unknown OrderStatus: "_s + pOrder->OrderStatus}});
    }
    send(TradeMsgCode::QUERY_ORDER,
        pRspInfo? json {
            {"code", pRspInfo->ErrorID},
            {"msg", pRspInfo->ErrorMsg}
        }: json(),
        pOrder? json {
            {"broker_id", pOrder->BrokerID},
            {"investor_id", pOrder->InvestorID},
            {"user_id", pOrder->UserID},
            {"exchange_id", pOrder->ExchangeID},
            {"req_id", pOrder->RequestID},
            {"ref", pOrder->OrderRef},
            {"order_local_id", pOrder->OrderLocalID},
            {"order_sys_id", pOrder->OrderSysID},
            {"sequence_no", pOrder->SequenceNo},
            {"instrument_id", pOrder->InstrumentID},
            {"insert_date", pOrder->InsertDate},
            {"insert_time", pOrder->InsertTime},
            {"active_time", pOrder->ActiveTime},
            {"suspend_time", pOrder->SuspendTime},
            {"update_time", pOrder->UpdateTime},
            {"cancel_time", pOrder->CancelTime},
            {"order_submit_status", submitStatus},
            {"order_status", status},
            {"volume_traded", pOrder->VolumeTraded},
            {"volume_total", pOrder->VolumeTotal},
            {"status_msg", u8(pOrder->StatusMsg)},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json()
    );
}

} // namespace tabxx