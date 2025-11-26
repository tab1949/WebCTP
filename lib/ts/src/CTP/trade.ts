import * as ws from "ws";
import * as Flags from "./flags";
import * as Message from "./message";

export class Trade {
    public onInit: (data: any) => void = () => {};
    public onPerformed: (data: any) => void = () => {};
    public onError: (data: any) => void = () => {};
    public onErrorNull: (data: any) => void = () => {};
    public onErrorUnknownValue: (data: any) => void = () => {};
    public onConnected: (data: any) => void = () => {};
    public onTradingDay: (data: any) => void = () => {};
    public onDisconnected: (data: any) => void = () => {};
    public onAuthenticate: (data: any) => void = () => {};
    public onLogin: (data: any) => void = () => {};
    public onLogout: (data: any) => void = () => {};
    public onSettlementInfo: (data: Message.SettlementInfo) => void = () => {};
    public onSettlementInfoConfirm: (data: Message.SettlementInfoConfirm) => void = () => {};
    public onTradingAccount: (data: Message.TradingAccount) => void = () => {};
    public onOrderInsertReturnError: (data: Message.OrderInsertReturnError) => void = () => {};
    public onOrderInsertError: (data: Message.OrderInsertError) => void = () => {};
    public onOrderInserted: (data: Message.OrderInserted) => void = () => {};
    public onOrderTraded: (data: Message.OrderTraded) => void = () => {};
    public onQueryOrder: (data: Message.QueryOrder) => void = () => {};
    public onOrderDeleteReturnError: (data: Message.OrderDeleteReturnError) => void = () => {};
    public onOrderDeleteError: (data: Message.OrderDeleteError) => void = () => {};
    public onOrderDeleted: (data: Message.OrderDeleted) => void = () => {};
    private brokerID: string;
    private investorID: string;
    private frontAddr: string | undefined;
    private frontPort: string | undefined;
    private ws: ws.WebSocket | undefined;
    private tradingDay: string | undefined;

    constructor(brokerID: string = "", investorID: string = "") {
        this.brokerID = brokerID;
        this.investorID = investorID;
    }

    public set(brokerID?: string, investorID?: string) {
        if (brokerID !== undefined) {
            this.brokerID = brokerID;
        }
        if (investorID !== undefined) {
            this.investorID = investorID;
        }
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        const data: any = {};
        if (brokerID !== undefined) {
            data.broker_id = brokerID;
        }
        if (investorID !== undefined) {
            data.investor_id = investorID;
        }
        this.ws.send(JSON.stringify({
            op: "set",
            data: data
        }));
    }

    public getTradingDay() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        this.ws.send(JSON.stringify({
            op: "get_trading_day",
            data: {}
        }));
    }

    public auth(userID: string, appID: string, authCode: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "auth",
            data: {
                user_id: userID,
                app_id: appID,
                auth_code: authCode
            }
        }));
    }

    public login(userID: string, password: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        this.ws.send(JSON.stringify({
            op: "login",
            data: {
                user_id: userID,
                password: password
            }
        }));
    }

    public logout(userID: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "logout",
            data: {
                user_id: userID
            }
        }));
    }

    public querySettlementInfo(tradingDay: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "query_settlement_info",
            data: {
                trading_day: tradingDay
            }
        }));
    }

    public confirmSettlementInfo() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        this.ws.send(JSON.stringify({
            op: "confirm_settlement_info",
            data: {}
        }));
    }

    public queryTradingAccount() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "query_trading_account",
            data: {}
        }));
    }

    public insertOrder(
        instrument: string,
        exchange: string,
        ref: string,
        price: number,
        direction: number,
        offset: number,
        volume: number,
        priceType: number,
        timeCondition: number
    ) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        this.ws.send(JSON.stringify({
            op: "insert_order",
            data: {
                instrument: instrument,
                exchange: exchange,
                ref: ref,
                price: price,
                direction: direction,
                offset: offset,
                volume: volume,
                price_type: priceType,
                time_condition: timeCondition
            }
        }));
    }

    public queryOrder(options?: {
        orderSysID?: string,
        exchangeID?: string,
        from?: string,
        to?: string
    }) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        const data: any = {};
        if (options) {
            if (options.orderSysID !== undefined) {
                data.order_sys_id = options.orderSysID;
            }
            if (options.exchangeID !== undefined) {
                data.exchange_id = options.exchangeID;
            }
            if (options.from !== undefined) {
                data.from = options.from;
            }
            if (options.to !== undefined) {
                data.to = options.to;
            }
        }
        this.ws.send(JSON.stringify({
            op: "query_order",
            data: data
        }));
    }

    public deleteOrder(exchange: string, instrument: string, delRef: number, orderSysID: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
        }
        this.ws.send(JSON.stringify({
            op: "delete_order",
            data: {
                exchange: exchange,
                instrument: instrument,
                delete_ref: delRef,
                order_sys_id: orderSysID
            }
        }));
    }

    public connectFront(addr: string, port: string) {
        this.ws?.send(JSON.stringify({
            op: "connect",
            data: {
                addr: addr,
                port: port
            }
        }));
    }

    public setFront(addr: string, port: string) {
        this.frontAddr = addr;
        this.frontPort = port;
    }

    public connect(addr: string, port: string) {
        if (this.ws) 
            return;
        if (!this.frontAddr || !this.frontPort) {
            throw new Error("Front address or port not set");
        }
        this.ws = new ws.WebSocket(`ws://${addr}:${port}/trade`);
        this.ws.onclose = () => {
            this.ws = undefined;
        };
        this.ws.onerror = (event) => {
            console.error("Error: " + JSON.stringify(event));
        };
        this.ws.onopen = () => {
            
        };
        this.ws.onmessage = (event) => {
            let data: any;
            try {
                data = JSON.parse(event.data.toString());
            } catch (error) {
                throw new Error("Parse message failed: " + error);
                return;
            }
            if (!this.ws) {
                throw new Error("WebSocket is not available");
                return;
            }
            switch (data.msg) {
            case Message.TradeMsgCode.PERFORMED:
                this.onPerformed(data);
                break;
            case Message.TradeMsgCode.ERROR:
                this.onError(data);
                break;
            case Message.TradeMsgCode.ERROR_NULL:
                this.onErrorNull(data);
                break;
            case Message.TradeMsgCode.ERROR_UNKNOWN_VALUE:
                this.onErrorUnknownValue(data);
                break;
            case Message.TradeMsgCode.CONNECTED:
                this.onConnected(data);
                break;
            case Message.TradeMsgCode.TRADING_DAY:
                if (data.info && data.info.trading_day) {
                    this.tradingDay = data.info.trading_day;
                }
                this.onTradingDay(data);
                break;
            case Message.TradeMsgCode.DISCONNECTED:
                this.onDisconnected(data);
                break;
            case Message.TradeMsgCode.AUTHENTICATE:
                this.onAuthenticate(data);
                break;
            case Message.TradeMsgCode.LOGIN:
                if (data.info && data.info.trading_day) {
                    this.tradingDay = data.info.trading_day;
                }
                this.onLogin(data);
                break;
            case Message.TradeMsgCode.LOGOUT:
                this.onLogout(data);
                break;
            case Message.TradeMsgCode.SETTLEMENT_INFO:
                if (data.info) {
                    const d: Message.SettlementInfo = {
                        TradingDay: data.info.trading_day,
                        SettlementID: data.info.settlement_id,
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        SequenceNo: data.info.sequence_no,
                        Content: data.info.content,
                        AccountID: data.info.account_id,
                        CurrencyID: data.info.currency_id,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onSettlementInfo(d);
                }
                break;
            case Message.TradeMsgCode.SETTLEMENT_INFO_CONFIRM:
                if (data.info) {
                    const d: Message.SettlementInfoConfirm = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        ConfirmDate: data.info.confirm_date,
                        ConfirmTime: data.info.confirm_time,
                        SettlementID: data.info.settlement_id,
                        AccountID: data.info.account_id,
                        CurrencyID: data.info.currency_id,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onSettlementInfoConfirm(d);
                }
                break;
            case Message.TradeMsgCode.TRADING_ACCOUNT:
                if (data.info) {
                    const d: Message.TradingAccount = {
                        BrokerID: data.info.broker_id,
                        AccountID: data.info.account_id,
                        PreMortgage: data.info.pre_mortgage,
                        PreCredit: data.info.pre_credit,
                        PreDeposit: data.info.pre_deposit,
                        PreBalance: data.info.pre_balance,
                        PreMargin: data.info.pre_margin,
                        InterestBase: data.info.interest_base,
                        Interest: data.info.interest,
                        Deposit: data.info.deposit,
                        Withdraw: data.info.withdraw,
                        FrozenMargin: data.info.frozen_margin,
                        FrozenCash: data.info.frozen_cash,
                        FrozenCommission: data.info.frozen_commission,
                        CurrentMargin: data.info.current_margin,
                        CashIn: data.info.cash_in,
                        Commission: data.info.commission,
                        CloseProfit: data.info.close_profit,
                        PositionProfit: data.info.position_profit,
                        Available: data.info.available,
                        WithdrawQuota: data.info.withdraw_quota,
                        TradingDay: data.info.trading_day,
                        SettlementID: data.info.settlement_id,
                        Credit: data.info.credit,
                        Mortgage: data.info.mortgage,
                        ExchangeMargin: data.info.exchange_margin,
                        DeliveryMargin: data.info.delivery_margin,
                        ExchangeDeliveryMargin: data.info.exchange_delivery_margin,
                        ReserveBalance: data.info.reserve_balance,
                        CurrencyID: data.info.currency_id,
                        PreFundMortgageIn: data.info.pre_fund_mortgage_in,
                        PreFundMortgageOut: data.info.pre_fund_mortgage_out,
                        FundMortgageIn: data.info.fund_mortgage_in,
                        FundMortgageOut: data.info.fund_mortgage_out,
                        FundMortgageAvailable: data.info.fund_mortgage_available,
                        MortgageableFund: data.info.mortgageable_fund,
                        SpecProductMargin: data.info.spec_product_margin,
                        SpecProductFrozenMargin: data.info.spec_product_frozen_margin,
                        SpecProductCommission: data.info.spec_product_commission,
                        SpecProductFrozenCommission: data.info.spec_product_frozen_commission,
                        SpecProductPositionProfit: data.info.spec_product_position_profit,
                        SpecProductCloseProfit: data.info.spec_product_close_profit,
                        SpecProductPositionProfitByAlg: data.info.spec_product_position_profit_by_alg,
                        SpecProductExchangeMargin: data.info.spec_product_exchange_margin,
                        BizType: data.info.biz_type,
                        FrozenSwap: data.info.frozen_swap,
                        RemainSwap: data.info.remain_swap,
                        OptionValue: data.info.option_value,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onTradingAccount(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_INSERT_RETURN_ERROR:
                if (data.info) {
                    const d: Message.OrderInsertReturnError = {
                        AccountID: data.info.account_id,
                        UserID: data.info.user_id,
                        InvestorID: data.info.investor_id,
                        BrokerID: data.info.broker_id,
                        ClientID: data.info.client_id,
                        CurrencyID: data.info.currency_id,
                        ExchangeID: data.info.exchange_id,
                        GTDDate: data.info.gtd_date,
                        InstrumentID: data.info.instrument_id,
                        IsAutoSuspend: data.info.is_auto_suspend,
                        IsSwapOrder: data.info.is_swap_order,
                        LimitPrice: data.info.limit_price,
                        StopPrice: data.info.stop_price,
                        VolumeTotalOriginal: data.info.volume_total_original,
                        MinVolume: data.info.min_volume,
                        OrderMemo: data.info.order_memo,
                        OrderRef: data.info.order_ref,
                        RequestID: data.info.request_id,
                        SessionReqSeq: data.info.session_req_seq,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        PriceType: data.info.price_type,
                        Hedge: data.info.hedge,
                        TimeCondition: data.info.time_condition,
                    };
                    this.onOrderInsertReturnError(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_INSERT_ERROR:
                if (data.info) {
                    const d: Message.OrderInsertError = {
                        AccountID: data.info.account_id,
                        UserID: data.info.user_id,
                        InvestorID: data.info.investor_id,
                        BrokerID: data.info.broker_id,
                        ClientID: data.info.client_id,
                        CurrencyID: data.info.currency_id,
                        ExchangeID: data.info.exchange_id,
                        GTDDate: data.info.gtd_date,
                        InstrumentID: data.info.instrument_id,
                        IsAutoSuspend: data.info.is_auto_suspend,
                        IsSwapOrder: data.info.is_swap_order,
                        LimitPrice: data.info.limit_price,
                        StopPrice: data.info.stop_price,
                        VolumeTotalOriginal: data.info.volume_total_original,
                        MinVolume: data.info.min_volume,
                        OrderMemo: data.info.order_memo,
                        OrderRef: data.info.order_ref,
                        RequestID: data.info.request_id,
                        SessionReqSeq: data.info.session_req_seq,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        PriceType: data.info.price_type,
                        Hedge: data.info.hedge,
                        TimeCondition: data.info.time_condition,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onOrderInsertError(d);
                }    
                break;
            case Message.TradeMsgCode.ORDER_INSERTED:
                if (data.info) {
                    const d: Message.OrderInserted = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        UserID: data.info.user_id,
                        ExchangeID: data.info.exchange_id,
                        ReqID: data.info.req_id,
                        Ref: data.info.ref,
                        OrderLocalID: data.info.order_local_id,
                        OrderSysID: data.info.order_sys_id,
                        SequenceNo: data.info.sequence_no,
                        InstrumentID: data.info.instrument_id,
                        InsertDate: data.info.insert_date,
                        InsertTime: data.info.insert_time,
                        ActiveTime: data.info.active_time,
                        SuspendTime: data.info.suspend_time,
                        UpdateTime: data.info.update_time,
                        CancelTime: data.info.cancel_time,
                        OrderSubmitStatus: data.info.order_submit_status,
                        OrderStatus: data.info.order_status,
                        VolumeTraded: data.info.volume_traded,
                        VolumeTotal: data.info.volume_total,
                        StatusMsg: data.info.status_msg,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        PriceType: data.info.price_type,
                        Hedge: data.info.hedge,
                        TimeCondition: data.info.time_condition,
                    };
                    this.onOrderInserted(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_TRADED:
                if (data.info) {
                    const d: Message.OrderTraded = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        UserID: data.info.user_id,
                        ExchangeID: data.info.exchange_id,
                        TradeID: data.info.trade_id,
                        OrderSysID: data.info.order_sys_id,
                        OrderLocalID: data.info.order_local_id,
                        BrokerOrderSeq: data.info.broker_order_seq,
                        SettlementID: data.info.settlement_id,
                        Volume: data.info.volume,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        Hedge: data.info.hedge,
                    };
                    this.onOrderTraded(d);
                }
                break;
            case Message.TradeMsgCode.QUERY_ORDER:
                if (data.info) {
                    const d: Message.QueryOrder = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        UserID: data.info.user_id,
                        ExchangeID: data.info.exchange_id,
                        RequestID: data.info.request_id,
                        Ref: data.info.ref,
                        OrderLocalID: data.info.order_local_id,
                        OrderSysID: data.info.order_sys_id,
                        SequenceNo: data.info.sequence_no,
                        InstrumentID: data.info.instrument_id,
                        InsertDate: data.info.insert_date,
                        InsertTime: data.info.insert_time,
                        ActiveTime: data.info.active_time,
                        SuspendTime: data.info.suspend_time,
                        UpdateTime: data.info.update_time,
                        CancelTime: data.info.cancel_time,
                        OrderSubmitStatus: data.info.order_submit_status,
                        OrderStatus: data.info.order_status,
                        VolumeTraded: data.info.volume_traded,
                        VolumeTotal: data.info.volume_total,
                        StatusMsg: data.info.status_msg,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        PriceType: data.info.price_type,
                        Hedge: data.info.hedge,
                        TimeCondition: data.info.time_condition,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onQueryOrder(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_DELETE_RETURN_ERROR:
                if (data.info) {
                    const d: Message.OrderDeleteReturnError = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        OrderActionRef: data.info.order_action_ref,
                        OrderRef: data.info.order_ref,
                        RequestID: data.info.request_id,
                        FrontID: data.info.front_id,
                        SessionID: data.info.session_id,
                        ExchangeID: data.info.exchange_id,
                        OrderSysID: data.info.order_sys_id,
                        ActionFlag: data.info.action_flag,
                        LimitPrice: data.info.limit_price,
                        VolumeChange: data.info.volume_change,
                        ActionDate: data.info.action_date,
                        ActionTime: data.info.action_time,
                        TraderID: data.info.trader_id,
                        InstallID: data.info.install_id,
                        OrderLocalID: data.info.order_local_id,
                        ActionLocalID: data.info.action_local_id,
                        ParticipantID: data.info.participant_id,
                        ClientID: data.info.client_id,
                        BusinessUnit: data.info.business_unit,
                        OrderActionStatus: data.info.order_action_status,
                        UserID: data.info.user_id,
                        StatusMsg: data.info.status_msg,
                        BranchID: data.info.branch_id,
                        InvestUnitID: data.info.invest_unit_id,
                        MacAddress: data.info.mac_address,
                        InstrumentID: data.info.instrument_id,
                        IPAddress: data.info.ip_address,
                        OrderMemo: data.info.order_memo,
                        SessionReqSeq: data.info.session_req_seq
                    };
                    this.onOrderDeleteReturnError(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_DELETE_ERROR:
                if (data.info) {
                    const d: Message.OrderDeleteError = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        OrderActionRef: data.info.order_action_ref,
                        OrderRef: data.info.order_ref,
                        RequestID: data.info.request_id,
                        FrontID: data.info.front_id,
                        SessionID: data.info.session_id,
                        ExchangeID: data.info.exchange_id,
                        OrderSysID: data.info.order_sys_id,
                        ActionFlag: data.info.action_flag,
                        LimitPrice: data.info.limit_price,
                        VolumeChange: data.info.volume_change,
                        UserID: data.info.user_id,
                        InvestUnitID: data.info.invest_unit_id,
                        MacAddress: data.info.mac_address,
                        InstrumentID: data.info.instrument_id,
                        IPAddress: data.info.ip_address,
                        OrderMemo: data.info.order_memo,
                        SessionReqSeq: data.info.session_req_seq,
                        ReqID: data.info.req_id,
                        IsLast: data.info.is_last,
                    };
                    this.onOrderDeleteError(d);
                }
                break;
            case Message.TradeMsgCode.ORDER_DELETED:
                if (data.info) {
                    const d: Message.OrderDeleted = {
                        BrokerID: data.info.broker_id,
                        InvestorID: data.info.investor_id,
                        UserID: data.info.user_id,
                        ExchangeID: data.info.exchange_id,
                        ReqID: data.info.req_id,
                        Ref: data.info.ref,
                        OrderLocalID: data.info.order_local_id,
                        OrderSysID: data.info.order_sys_id,
                        SequenceNo: data.info.sequence_no,
                        InstrumentID: data.info.instrument_id,
                        InsertDate: data.info.insert_date,
                        InsertTime: data.info.insert_time,
                        ActiveTime: data.info.active_time,
                        SuspendTime: data.info.suspend_time,
                        UpdateTime: data.info.update_time,
                        CancelTime: data.info.cancel_time,
                        OrderSubmitStatus: data.info.order_submit_status,
                        OrderStatus: data.info.order_status,
                        VolumeTraded: data.info.volume_traded,
                        VolumeTotal: data.info.volume_total,
                        StatusMsg: data.info.status_msg,
                        Direction: data.info.direction,
                        Offset: data.info.offset,
                        PriceType: data.info.price_type,
                        Hedge: data.info.hedge,
                        TimeCondition: data.info.time_condition,
                    };
                    this.onOrderDeleted(d);
                }
                break;
            default:
                if (data.status && data.status == "ready") {
                    this.onInit(data);
                    if (this.ws) {
                        this.ws.send(JSON.stringify({
                            op: "connect",
                            data: {
                                addr: this.frontAddr,
                                port: this.frontPort
                            }
                        }));
                    }
                    break;
                }
                throw new Error("Unknown message: " + JSON.stringify(data));
            }
        };
    }
}; // class Trade