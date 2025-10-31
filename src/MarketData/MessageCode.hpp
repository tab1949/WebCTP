#ifndef TABXX_MARKET_DATA_MESSAGE_CODE_HPP_
#define TABXX_MARKET_DATA_MESSAGE_CODE_HPP_

namespace tabxx {

enum class MDMsgCode {
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
};

} // namespace tabxx
#endif // TABXX_MARKET_DATA_MESSAGE_CODE_HPP_