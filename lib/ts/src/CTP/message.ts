import * as Flags from "./flags";

export enum MDMsgCode {
    PERFORMED = 0,
    ERROR = 1,
    CONNECTED = 2,
    DISCONNECTED = 3,
    HEARTBEAT_TIMEOUT = 4,
    LOGIN = 5,
    LOGOUT = 6,
    TRADING_DAY = 7,
    SUBSCRIBE = 8,
    UNSUBSCRIBE = 9,
    MARKET_DATA = 10
}

export const MDMsgInfo: Record<MDMsgCode, string> = {
    [MDMsgCode.PERFORMED]: "Performed",
    [MDMsgCode.ERROR]: "Error",
    [MDMsgCode.CONNECTED]: "Connected",
    [MDMsgCode.DISCONNECTED]: "Disconnected",
    [MDMsgCode.HEARTBEAT_TIMEOUT]: "Heartbeat Timeout",
    [MDMsgCode.LOGIN]: "Login",
    [MDMsgCode.LOGOUT]: "Logout",
    [MDMsgCode.TRADING_DAY]: "Trading Day",
    [MDMsgCode.SUBSCRIBE]: "Subscribe",
    [MDMsgCode.UNSUBSCRIBE]: "Unsubscribe",
    [MDMsgCode.MARKET_DATA]: "Market Data"
}

export enum TradeMsgCode {
    PERFORMED = 0,
    ERROR,
    ERROR_NULL,
    ERROR_UNKNOWN_VALUE,
    CONNECTED,
    TRADING_DAY,
    DISCONNECTED,
    AUTHENTICATE,
    LOGIN,
    LOGOUT,
    SETTLEMENT_INFO,
    SETTLEMENT_INFO_CONFIRM,
    TRADING_ACCOUNT,
    ORDER_INSERT_ERROR,
    ORDER_INSERT_RETURN_ERROR,
    ORDER_INSERTED,
    ORDER_TRADED,
    QUERY_ORDER,
    ORDER_DELETE_ERROR,
    ORDER_DELETE_RETURN_ERROR,
    ORDER_DELETED
}

export const TradeMsgInfo: Record<TradeMsgCode, string> = {
    [TradeMsgCode.PERFORMED]: "Performed",
    [TradeMsgCode.ERROR]: "Error",
    [TradeMsgCode.ERROR_NULL]: "NULL Pointer",
    [TradeMsgCode.ERROR_UNKNOWN_VALUE]: "Unknown Value",
    [TradeMsgCode.CONNECTED]: "Connected",
    [TradeMsgCode.TRADING_DAY]: "Trading Day",
    [TradeMsgCode.DISCONNECTED]: "Disconnected",
    [TradeMsgCode.AUTHENTICATE]: "Authenticate",
    [TradeMsgCode.LOGIN]: "Login",
    [TradeMsgCode.LOGOUT]: "Logout",
    [TradeMsgCode.SETTLEMENT_INFO]: "Settlement Info",
    [TradeMsgCode.SETTLEMENT_INFO_CONFIRM]: "Settlement Info Confirm",
    [TradeMsgCode.TRADING_ACCOUNT]: "Trading Account",
    [TradeMsgCode.ORDER_INSERT_ERROR]: "Order Insert Error",
    [TradeMsgCode.ORDER_INSERT_RETURN_ERROR]: "Order Insert Returned Error",
    [TradeMsgCode.ORDER_INSERTED]: "Order Inserted",
    [TradeMsgCode.ORDER_TRADED]: "Order Traded",
    [TradeMsgCode.QUERY_ORDER]: "Query Order",
    [TradeMsgCode.ORDER_DELETE_ERROR]: "Failed to delete order",
    [TradeMsgCode.ORDER_DELETE_RETURN_ERROR]: "Delete order returned error",
    [TradeMsgCode.ORDER_DELETED]: "Order Deleted"
}

export interface MarketData {
    TradingDay: string;
    InstrumentID: string;
    ExchangeID: string;
    ExchangeInstID: string;
    LastPrice: number;
    PreSettlementPrice: number;
    PreClosePrice: number;
    PreOpenInterest: number;
    OpenPrice: number;
    HighestPrice: number;
    LowestPrice: number;
    Volume: number;
    Turnover: number;
    OpenInterest: number;
    ClosePrice: number;
    SettlementPrice: number;
    UpperLimitPrice: number;
    LowerLimitPrice: number;
    PreDelta: number;
    CurrDelta: number;
    UpdateTime: string;
    UpdateMillisec: number;
    BidPrice1: number;
    BidVolume1: number;
    AskPrice1: number;
    AskVolume1: number;
    BidPrice2: number;
    BidVolume2: number;
    AskPrice2: number;
    AskVolume2: number;
    BidPrice3: number;
    BidVolume3: number;
    AskPrice3: number;
    AskVolume3: number;
    BidPrice4: number;
    BidVolume4: number;
    AskPrice4: number;
    AskVolume4: number;
    BidPrice5: number;
    BidVolume5: number;
    AskPrice5: number;
    AskVolume5: number;
    AveragePrice: number;
    ActionDay: string;
    BandingUpperPrice: number;
    BandingLowerPrice: number;
}

export interface TradingAccount {
    BrokerID: string;
    AccountID: string;
    PreMortgage: number;
    PreCredit: number;
    PreDeposit: number;
    PreBalance: number;
    PreMargin: number;
    InterestBase: number;
    Interest: number;
    Deposit: number;
    Withdraw: number;
    FrozenMargin: number;
    FrozenCash: number;
    FrozenCommission: number;
    CurrentMargin: number;
    CashIn: number;
    Commission: number;
    CloseProfit: number;
    PositionProfit: number;
    Available: number;
    WithdrawQuota: number;
    TradingDay: string;
    SettlementID: string;
    Credit: number;
    Mortgage: number;
    ExchangeMargin: number;
    DeliveryMargin: number;
    ExchangeDeliveryMargin: number;
    ReserveBalance: number;
    CurrencyID: string;
    PreFundMortgageIn: number;
    PreFundMortgageOut: number;
    FundMortgageIn: number;
    FundMortgageOut: number;
    FundMortgageAvailable: number;
    MortgageableFund: number;
    SpecProductMargin: number;
    SpecProductFrozenMargin: number;
    SpecProductCommission: number;
    SpecProductFrozenCommission: number;
    SpecProductPositionProfit: number;
    SpecProductCloseProfit: number;
    SpecProductPositionProfitByAlg: number;
    SpecProductExchangeMargin: number;
    BizType: string;
    FrozenSwap: number;
    RemainSwap: number;
    OptionValue: number;
    ReqID: number;
    IsLast: boolean;
}

export interface SettlementInfo {
    TradingDay: string;
    SettlementID: string;
    BrokerID: string;
    InvestorID: string;
    SequenceNo: number;
    Content: string;
    AccountID: string;
    CurrencyID: string;
    ReqID: number;
    IsLast: boolean;
}

export interface SettlementInfoConfirm {
    BrokerID: string;
    InvestorID: string;
    ConfirmDate: string;
    ConfirmTime: string;
    SettlementID: number;
    AccountID: string;
    CurrencyID: string;
    ReqID: number;
    IsLast: boolean;
}

export interface OrderInserted {
    BrokerID: string;
    InvestorID: string;
    UserID: string;
    ExchangeID: string;
    ReqID: number;
    Ref: string;
    OrderLocalID: string;
    OrderSysID: string;
    SequenceNo: number;
    InstrumentID: string;
    InsertDate: string;
    InsertTime: string;
    ActiveTime: string;
    SuspendTime: string;
    UpdateTime: string;
    CancelTime: string;
    OrderSubmitStatus: Flags.OrderSubmitStatus;
    OrderStatus: Flags.OrderStatus;
    VolumeTraded: number;
    VolumeTotal: number;
    StatusMsg: string;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    PriceType: Flags.OrderPriceType;
    Hedge: Flags.Hedge;
    TimeCondition: Flags.TimeCondition;
}

export interface OrderInsertError {
    AccountID: string;
    UserID: string;
    InvestorID: string;
    BrokerID: string;
    ClientID: string;
    CurrencyID: string;
    ExchangeID: string;
    GTDDate: string;
    InstrumentID: string;
    IsAutoSuspend: boolean;
    IsSwapOrder: boolean;
    LimitPrice: number;
    StopPrice: number;
    VolumeTotalOriginal: number;
    MinVolume: number;
    OrderMemo: string;
    OrderRef: string;
    RequestID: number;
    SessionReqSeq: number;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    PriceType: Flags.OrderPriceType;
    Hedge: Flags.Hedge;
    TimeCondition: Flags.TimeCondition;
    ReqID: number;
    IsLast: boolean;
}

export interface OrderInsertReturnError {
    AccountID: string;
    UserID: string;
    InvestorID: string;
    BrokerID: string;
    ClientID: string;
    CurrencyID: string;
    ExchangeID: string;
    GTDDate: string;
    InstrumentID: string;
    IsAutoSuspend: boolean;
    IsSwapOrder: boolean;
    LimitPrice: number;
    StopPrice: number;
    VolumeTotalOriginal: number;
    MinVolume: number;
    OrderMemo: string;
    OrderRef: string;
    RequestID: number;
    SessionReqSeq: number;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    PriceType: Flags.OrderPriceType;
    Hedge: Flags.Hedge;
    TimeCondition: Flags.TimeCondition;
}

export interface OrderDeleted {
    BrokerID: string;
    InvestorID: string;
    UserID: string;
    ExchangeID: string;
    ReqID: number;
    Ref: string;
    OrderLocalID: string;
    OrderSysID: string;
    SequenceNo: number;
    InstrumentID: string;
    InsertDate: string;
    InsertTime: string;
    ActiveTime: string;
    SuspendTime: string;
    UpdateTime: string;
    CancelTime: string;
    OrderSubmitStatus: Flags.OrderSubmitStatus;
    OrderStatus: Flags.OrderStatus;
    VolumeTraded: number;
    VolumeTotal: number;
    StatusMsg: string;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    PriceType: Flags.OrderPriceType;
    Hedge: Flags.Hedge;
    TimeCondition: Flags.TimeCondition;
}

export interface OrderDeleteError {
    BrokerID: string;
    InvestorID: string;
    OrderActionRef: number;
    OrderRef: string;
    RequestID: number;
    FrontID: number;
    SessionID: number;
    ExchangeID: string;
    OrderSysID: string;
    ActionFlag: string;
    LimitPrice: number;
    VolumeChange: number;
    UserID: string;
    InvestUnitID: string;
    MacAddress: string;
    InstrumentID: string;
    IPAddress: string;
    OrderMemo: string;
    SessionReqSeq: number;
    ReqID: number;
    IsLast: boolean;
}

export interface OrderDeleteReturnError {
    BrokerID: string;
    InvestorID: string;
    OrderActionRef: number;
    OrderRef: string;
    RequestID: number;
    FrontID: number;
    SessionID: number;
    ExchangeID: string;
    OrderSysID: string;
    ActionFlag: string;
    LimitPrice: number;
    VolumeChange: number;
    ActionDate: string;
    ActionTime: string;
    TraderID: string;
    InstallID: number;
    OrderLocalID: string;
    ActionLocalID: string;
    ParticipantID: string;
    ClientID: string;
    BusinessUnit: string;
    OrderActionStatus: string;
    UserID: string;
    StatusMsg: string;
    BranchID: string;
    InvestUnitID: string;
    MacAddress: string;
    InstrumentID: string;
    IPAddress: string;
    OrderMemo: string;
    SessionReqSeq: number;
}

export interface OrderTraded {
    BrokerID: string;
    InvestorID: string;
    UserID: string;
    ExchangeID: string;
    TradeID: string;
    OrderSysID: string;
    OrderLocalID: string;
    BrokerOrderSeq: string;
    SettlementID: string;
    Volume: number;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    Hedge: Flags.Hedge;
}

export interface QueryOrder {
    BrokerID: string;
    InvestorID: string;
    UserID: string;
    ExchangeID: string;
    RequestID: number;
    Ref: string;
    OrderLocalID: string;
    OrderSysID: string;
    SequenceNo: number;
    InstrumentID: string;
    InsertDate: string;
    InsertTime: string;
    ActiveTime: string;
    SuspendTime: string;
    UpdateTime: string;
    CancelTime: string;
    OrderSubmitStatus: Flags.OrderSubmitStatus;
    OrderStatus: Flags.OrderStatus;
    VolumeTraded: number;
    VolumeTotal: number;
    StatusMsg: string;
    Direction: Flags.Direction;
    Offset: Flags.OrderOffset;
    PriceType: Flags.OrderPriceType;
    Hedge: Flags.Hedge;
    TimeCondition: Flags.TimeCondition;
    ReqID: number;
    IsLast: boolean;
}