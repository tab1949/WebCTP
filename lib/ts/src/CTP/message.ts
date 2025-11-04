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