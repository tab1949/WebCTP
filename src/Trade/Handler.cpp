#include "Handler.hpp"

namespace tabxx {

void TraderHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::ERROR, pRspInfo, {
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
    send(TradeMsgCode::AUTHENTICATE, pRspInfo, pRspAuthenticateField? json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"app_id", pRspAuthenticateField->AppID},
            {"app_type", pRspAuthenticateField->AppType},
            {"broker_id", pRspAuthenticateField->BrokerID},
            {"user_id", pRspAuthenticateField->UserID},
            {"user_product_info", u8(pRspAuthenticateField->UserProductInfo)}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)  {
    send(TradeMsgCode::LOGIN, pRspInfo, pRspUserLogin? json {
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
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast) {
    send(TradeMsgCode::LOGOUT, pRspInfo, pUserLogout? json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
            {"broker_id", pUserLogout->BrokerID},
            {"user_id", pUserLogout->UserID}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspQrySettlementInfo(
    CThostFtdcSettlementInfoField *pSettlementInfo, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::SETTLEMENT_INFO, pRspInfo, pSettlementInfo? json {
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
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::SETTLEMENT_INFO_CONFIRM, pRspInfo,
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
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::TRADING_ACCOUNT, pRspInfo,
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
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast},
        }
    );
}

void TraderHandler::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    Direction direction;
    OrderOffset of;
    OrderPriceType opt;
    Hedge hedge;
    TimeCondition tc;
    if (pInputOrder) {
        try {
            direction = GetDirection(pInputOrder->Direction);
            of = GetOrderOperation(pInputOrder->CombOffsetFlag[0]);
            opt = GetOrderPriceType(pInputOrder->OrderPriceType);
            hedge = GetHedge(pInputOrder->CombHedgeFlag[0]);
            tc = GetTimeCondition(pInputOrder->TimeCondition);
        }
        catch (const std::exception& e) {
            logger_->error("tabxx::TraderHandler::OnRspOrderInsert(): Exception caught");
            send(TradeMsgCode::ERROR_UNKNOWN_VALUE, pRspInfo, {
                {"info", e.what()},
                {"req_id", nRequestID},
                {"is_last", bIsLast}
            });
            logger_->error("tabxx::TraderHandler::OnRspOrderInsert(): what(): "_s + e.what());
        }
    }
    else {
        logger_->error("tabxx::TraderHandler::OnRspOrderInsert(): pInputOrder is nullptr");
    }
    send(TradeMsgCode::ORDER_INSERT_ERROR, pRspInfo,
        pInputOrder? json {
            {"account_id", pInputOrder->AccountID},
            {"user_id", pInputOrder->UserID},
            {"investor_id", pInputOrder->InvestorID},
            {"broker_id", pInputOrder->BrokerID},
            {"client_id", pInputOrder->ClientID},
            {"currency_id", pInputOrder->CurrencyID},
            {"exchange_id", pInputOrder->ExchangeID},
            {"gtd_date", pInputOrder->GTDDate},
            {"instrument_id", pInputOrder->InstrumentID},
            {"is_auto_suspend", pInputOrder->IsAutoSuspend},
            {"is_swap_order", pInputOrder->IsSwapOrder},
            {"limit_price", pInputOrder->LimitPrice},
            {"stop_price", pInputOrder->StopPrice},
            {"volume_total_original", pInputOrder->VolumeTotalOriginal},
            {"min_volume", pInputOrder->MinVolume},
            {"order_memo", pInputOrder->OrderMemo},
            {"order_ref", pInputOrder->OrderRef},
            {"request_id", pInputOrder->RequestID},
            {"session_req_seq_", pInputOrder->SessionReqSeq},
            {"direction", direction},
            {"offset", of},
            {"price_type", opt},
            {"hedge", hedge},
            {"time_condition", tc},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void TraderHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
    Direction direction;
    OrderOffset of;
    OrderPriceType opt;
    Hedge hedge;
    TimeCondition tc;
    if (pInputOrder) {
        try {
            direction = GetDirection(pInputOrder->Direction);
            of = GetOrderOperation(pInputOrder->CombOffsetFlag[0]);
            opt = GetOrderPriceType(pInputOrder->OrderPriceType);
            hedge = GetHedge(pInputOrder->CombHedgeFlag[0]);
            tc = GetTimeCondition(pInputOrder->TimeCondition);
        }
        catch (const std::exception& e) {
            logger_->error("tabxx::TraderHandler::OnErrRtnOrderInsert(): Exception caught");
            send(TradeMsgCode::ERROR_UNKNOWN_VALUE, pRspInfo, {
                {"info", e.what()},
            });
            logger_->error("tabxx::TraderHandler::OnErrRtnOrderInsert(): what(): "_s + e.what());
        }
    }
    else {
        logger_->error("tabxx::TraderHandler::OnErrRtnOrderInsert(): pInputOrder is nullptr");
    }
    send(TradeMsgCode::ORDER_INSERT_RETURN_ERROR, pRspInfo,
        pInputOrder? json {
            {"account_id", pInputOrder->AccountID},
            {"user_id", pInputOrder->UserID},
            {"investor_id", pInputOrder->InvestorID},
            {"broker_id", pInputOrder->BrokerID},
            {"client_id", pInputOrder->ClientID},
            {"currency_id", pInputOrder->CurrencyID},
            {"exchange_id", pInputOrder->ExchangeID},
            {"gtd_date", pInputOrder->GTDDate},
            {"instrument_id", pInputOrder->InstrumentID},
            {"is_auto_suspend", pInputOrder->IsAutoSuspend},
            {"is_swap_order", pInputOrder->IsSwapOrder},
            {"limit_price", pInputOrder->LimitPrice},
            {"stop_price", pInputOrder->StopPrice},
            {"volume_total_original", pInputOrder->VolumeTotalOriginal},
            {"min_volume", pInputOrder->MinVolume},
            {"order_memo", pInputOrder->OrderMemo},
            {"order_ref", pInputOrder->OrderRef},
            {"request_id", pInputOrder->RequestID},
            {"session_req_seq", pInputOrder->SessionReqSeq},
            {"direction", direction},
            {"offset", of},
            {"price_type", opt},
            {"hedge", hedge},
            {"time_condition", tc}
        }: json()
    );
}

void TraderHandler::OnRtnOrder(CThostFtdcOrderField *pOrder) {
    OrderSubmitStatus submitStatus;
    OrderStatus status;
    Direction direction;
    OrderOffset of;
    OrderPriceType opt;
    Hedge hedge;
    TimeCondition tc;
    if (pOrder) {
        try {
            submitStatus = GetOrderSubmitStatus(pOrder->OrderSubmitStatus);
            status = GetOrderStatus(pOrder->OrderStatus);
            direction = GetDirection(pOrder->Direction);
            of = GetOrderOperation(pOrder->CombOffsetFlag[0]);
            opt = GetOrderPriceType(pOrder->OrderPriceType);
            hedge = GetHedge(pOrder->CombHedgeFlag[0]);
            tc = GetTimeCondition(pOrder->TimeCondition);
        }
        catch (const std::exception& e) {
            logger_->error("tabxx::TraderHandler::OnRtnOrder(): Exception caught.");
            send(TradeMsgCode::ERROR_UNKNOWN_VALUE, {}, {
                {"info", e.what()}
            });
            logger_->error("tabxx::TraderHandler::OnRtnOrder(): what(): "_s + e.what());
        }
    }
    else {
        logger_->error("tabxx::TraderHandler::OnRtnOrder(): Parameter 'pOrder' is nullptr!");
    }
    send(status == OrderStatus::CANCELED ? TradeMsgCode::ORDER_DELETED: TradeMsgCode::ORDER_INSERTED,
        {
            {"code", 0},
            {"msg", ""}
        },
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
            {"direction", direction},
            {"offset", of},
            {"price_type", opt},
            {"hedge", hedge},
            {"time_condition", tc}
        }: json()
    );
}

void TraderHandler::OnRtnTrade(CThostFtdcTradeField *pTrade) {
    Direction direction;
    OrderOffset of;
    Hedge hedge;
    if (pTrade) {
        try {
            direction = GetDirection(pTrade->Direction);
            of = GetOrderOperation(pTrade->OffsetFlag);
            hedge = GetHedge(pTrade->HedgeFlag);
        }
        catch (const std::exception& e) {
            logger_->error("tabxx::TraderHandler::OnRtnTrade(): Exception caught.");
            send(TradeMsgCode::ERROR_UNKNOWN_VALUE, {}, {
                {"info", e.what()}
            });
            logger_->error("tabxx::TraderHandler::OnRtnTrade(): what(): "_s + e.what());
        }
    }
    else {
        logger_->error("tabxx::TraderHandler::OnRtnTrade(): Parameter 'pTrade' is nullptr!");
    }
    send(TradeMsgCode::ORDER_TRADED,
        {
            {"code", 0},
            {"msg", ""}
        },
        pTrade? json {
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
            {"direction", direction},
            {"offset", of},
            {"hedge", hedge}
        }: json()
    );

}

void TraderHandler::OnRspQryOrder(
    CThostFtdcOrderField *pOrder, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    OrderSubmitStatus submitStatus;
    OrderStatus status;
    Direction direction;
    OrderOffset of;
    OrderPriceType opt;
    Hedge hedge;
    TimeCondition tc;
    if (pOrder) {
        try {
            submitStatus = GetOrderSubmitStatus(pOrder->OrderSubmitStatus);
            status = GetOrderStatus(pOrder->OrderStatus);
            direction = GetDirection(pOrder->Direction);
            of = GetOrderOperation(pOrder->CombOffsetFlag[0]);
            opt = GetOrderPriceType(pOrder->OrderPriceType);
            hedge = GetHedge(pOrder->CombHedgeFlag[0]);
            tc = GetTimeCondition(pOrder->TimeCondition);
        }
        catch (const std::exception& e) {
            logger_->error((std::string)"tabxx::TraderHandler::OnRspQryOrder(): Exception caught.");
            send(TradeMsgCode::ERROR_UNKNOWN_VALUE, {}, {
                {"info", e.what()}
            });
            logger_->error((std::string)"tabxx::TraderHandler::OnRspQryOrder(): what(): " + e.what());
        }
    }
    send(TradeMsgCode::QUERY_ORDER, pRspInfo,
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
            {"order_memo", pOrder->OrderMemo},
            {"volume_traded", pOrder->VolumeTraded},
            {"volume_total", pOrder->VolumeTotal},
            {"status_msg", u8(pOrder->StatusMsg)},
            {"direction", direction},
            {"offset", of},
            {"price_type", opt},
            {"hedge", hedge},
            {"time_condition", tc},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void TraderHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    send(TradeMsgCode::ORDER_DELETE_ERROR, pRspInfo,
        pInputOrderAction? json {
            {"broker_id", pInputOrderAction->BrokerID},
            {"investor_id", pInputOrderAction->InvestorID},
            {"order_action_ref", pInputOrderAction->OrderActionRef},
            {"order_ref", pInputOrderAction->OrderRef},
            {"req_id", pInputOrderAction->RequestID},
            {"front_id", pInputOrderAction->FrontID},
            {"session_id", pInputOrderAction->SessionID},
            {"exchange_id", pInputOrderAction->ExchangeID},
            {"order_sys_id", pInputOrderAction->OrderSysID},
            {"action_flag", pInputOrderAction->ActionFlag},
            {"limit_price", pInputOrderAction->LimitPrice},
            {"volume_change", pInputOrderAction->VolumeChange},
            {"user_id", pInputOrderAction->UserID},
            {"invest_unit_id", pInputOrderAction->InvestUnitID},
            {"mac_address", pInputOrderAction->MacAddress},
            {"instrument_id", pInputOrderAction->InstrumentID},
            {"ip_address", pInputOrderAction->IPAddress},
            {"order_memo", pInputOrderAction->OrderMemo},
            {"session_req_seq", pInputOrderAction->SessionReqSeq},
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }: json {
            {"req_id", nRequestID},
            {"is_last", bIsLast}
        }
    );
}

void TraderHandler::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
    send(TradeMsgCode::ORDER_DELETE_RETURN_ERROR, pRspInfo,
        pOrderAction ? json {
            {"broker_id", pOrderAction->BrokerID},
            {"investor_id", pOrderAction->InvestorID},
            {"order_action_ref", pOrderAction->OrderActionRef},
            {"order_ref", pOrderAction->OrderRef},
            {"req_id", pOrderAction->RequestID},
            {"front_id", pOrderAction->FrontID},
            {"session_id", pOrderAction->SessionID},
            {"exchange_id", pOrderAction->ExchangeID},
            {"order_sys_id", pOrderAction->OrderSysID},
            {"action_flag", pOrderAction->ActionFlag},
            {"limit_price", pOrderAction->LimitPrice},
            {"volume_change", pOrderAction->VolumeChange},
            {"action_date", pOrderAction->ActionDate},
            {"action_time", pOrderAction->ActionTime},
            {"trader_id", pOrderAction->TraderID},
            {"install_id", pOrderAction->InstallID},
            {"order_local_id", pOrderAction->OrderLocalID},
            {"action_local_id", pOrderAction->ActionLocalID},
            {"participant_id", pOrderAction->ParticipantID},
            {"client_id", pOrderAction->ClientID},
            {"business_unit", pOrderAction->BusinessUnit},
            {"order_action_status", pOrderAction->OrderActionStatus},
            {"user_id", pOrderAction->UserID},
            {"status_msg", u8(pOrderAction->StatusMsg)},
            {"branch_id", pOrderAction->BranchID},
            {"invest_unit_id", pOrderAction->InvestUnitID},
            {"mac_address", pOrderAction->MacAddress},
            {"instrument_id", pOrderAction->InstrumentID},
            {"ip_address", pOrderAction->IPAddress},
            {"order_memo", pOrderAction->OrderMemo},
            {"session_req_seq", pOrderAction->SessionReqSeq}
        } : json {}
    );

}

} // namespace tabxx