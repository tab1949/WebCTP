#ifndef TABXX_TRADE_FLAGS_HPP_
#define TABXX_TRADE_FLAGS_HPP_

#include <exception>
#include <stdexcept>
#include <string>

#include <ThostFtdcUserApiDataType.h>

namespace tabxx {

enum class Direction {
    BUY = 0, 
    SELL = 1
};

inline Direction GetDirection(TThostFtdcDirectionType d) {
    switch (d) {
    case THOST_FTDC_D_Buy:
        return Direction::BUY;
    case THOST_FTDC_D_Sell:
        return Direction::SELL;
    default:
        throw std::runtime_error(std::string("tabxx::GetDirection(): Unknown order direction: ") + d);
    }
}

enum class OrderOffset {
    OPEN = 0, 
    CLOSE = 1,
    FORCED_CLOSE = 2,
    CLOSE_TODAY = 3,
    CLOSE_YESTERDAY = 4,
    FORCED_OFF = 5,
    LOCAL_FORCED_CLOSE = 6 
};

inline OrderOffset GetOrderOperation(TThostFtdcOffsetFlagType o) {
    switch (o) {
    case THOST_FTDC_OF_Open:
        return OrderOffset::OPEN;
    case THOST_FTDC_OF_Close:
        return OrderOffset::CLOSE;
    case THOST_FTDC_OF_CloseToday:
        return OrderOffset::CLOSE_TODAY;
    case THOST_FTDC_OF_CloseYesterday:
        return OrderOffset::CLOSE_YESTERDAY;
    case THOST_FTDC_OF_ForceClose:
        return OrderOffset::FORCED_CLOSE;
    case THOST_FTDC_OF_ForceOff:
        return OrderOffset::FORCED_OFF;
    case THOST_FTDC_OF_LocalForceClose:
        return OrderOffset::LOCAL_FORCED_CLOSE;
    default: 
        throw std::runtime_error(std::string("tabxx::GetOrderOperation(): Unknown order operation type: ") + o);
    }
}

enum class OrderPriceType {
    LIMITED = 0, 
    MARKET = 1,
    LAST = 2
};

inline OrderPriceType GetOrderPriceType(TThostFtdcOrderPriceTypeType o) {
    switch (o) {
    case THOST_FTDC_OPT_LimitPrice:
        return OrderPriceType::LIMITED;
    case THOST_FTDC_OPT_AnyPrice:
        return OrderPriceType::MARKET;
    case THOST_FTDC_OPT_LastPrice:
        return OrderPriceType::LAST;
    default:
        throw std::runtime_error(std::string("tabxx::GetOrderPriceType(): Unknown order price type: ") + o);
    }
}

enum class Hedge {
    SPECULATION = 0,
};

inline Hedge GetHedge(TThostFtdcHedgeFlagType h) {
    switch (h) {
    case THOST_FTDC_HF_Speculation:
        return Hedge::SPECULATION;
    default:
        throw std::runtime_error((std::string)"tabxx::GetHedge(): Unknown hedge type: " + h);
    }
}

enum class TimeCondition {
    IMMEDIATE = 0,
    ONE_DAY = 1
};

inline TimeCondition GetTimeCondition(TThostFtdcTimeConditionType tc) {
    switch (tc) {
    case THOST_FTDC_TC_IOC:
        return TimeCondition::IMMEDIATE;
    case THOST_FTDC_TC_GFD:
        return TimeCondition::ONE_DAY;
    default: 
        throw std::runtime_error((std::string)"tabxx::GetTimeCondition(): Unknown time condition: " + tc);
    }
}

enum class OrderSubmitStatus {
    INSERT_SUBMITTED = 0,
    CANCEL_SUBMITTED = 1,
    MODIFY_SUBMITTED = 2,
    ACCEPTED = 3,
    INSERT_REJECTED = 4,
    CANCEL_REJECTED = 5,
    MODIFY_REJECTED = 6
};

inline OrderSubmitStatus GetOrderSubmitStatus(TThostFtdcOrderSubmitStatusType s) {
    switch (s) {
    case THOST_FTDC_OSS_InsertSubmitted:
        return OrderSubmitStatus::INSERT_SUBMITTED;
    case THOST_FTDC_OSS_CancelSubmitted:
        return OrderSubmitStatus::CANCEL_SUBMITTED;
    case THOST_FTDC_OSS_ModifySubmitted:
        return OrderSubmitStatus::MODIFY_SUBMITTED;
    case THOST_FTDC_OSS_Accepted:
        return OrderSubmitStatus::ACCEPTED;
    case THOST_FTDC_OSS_InsertRejected:
        return OrderSubmitStatus::INSERT_REJECTED;
    case THOST_FTDC_OSS_CancelRejected:
        return OrderSubmitStatus::CANCEL_REJECTED;
    case THOST_FTDC_OSS_ModifyRejected:
        return OrderSubmitStatus::MODIFY_REJECTED;
    default: 
        throw std::runtime_error((std::string)"tabxx::GetOrderSubmitStatus(): Unknown order submitting status: " + s);
    }
}

enum class OrderStatus {
    ALL_TRADED = 0,
    PART_TRADED_QUEUEING = 1,
    PART_TRADED_NOT_QUEUEING = 2,
    NOT_TRADED_QUEUEING = 3,
    NOT_TRADED_NOT_QUEUEING = 4,
    CANCELED = 5,
    UNKNOWN = 6,
    NOT_TOUCHED = 7,
    TOUCHED = 8
};

inline OrderStatus GetOrderStatus(TThostFtdcOrderStatusType s) {
    switch (s) {
    case THOST_FTDC_OST_AllTraded:
        return OrderStatus::ALL_TRADED;
    case THOST_FTDC_OST_PartTradedQueueing:
        return OrderStatus::PART_TRADED_QUEUEING;
    case THOST_FTDC_OST_PartTradedNotQueueing:
        return OrderStatus::PART_TRADED_NOT_QUEUEING;
    case THOST_FTDC_OST_NoTradeQueueing:
        return OrderStatus::NOT_TRADED_QUEUEING;
    case THOST_FTDC_OST_NoTradeNotQueueing:
        return OrderStatus::NOT_TRADED_NOT_QUEUEING;
    case THOST_FTDC_OST_Canceled:
        return OrderStatus::CANCELED;
    case THOST_FTDC_OST_Unknown:
        return OrderStatus::UNKNOWN;
    case THOST_FTDC_OST_NotTouched:
        return OrderStatus::NOT_TOUCHED;
    case THOST_FTDC_OST_Touched:
        return OrderStatus::TOUCHED;
    default: 
        throw std::runtime_error((std::string)"tabxx::GetOrderStatus(): Unknown order status: " + s);
    }
}

} // namespace tabxx

#endif // TABXX_TRADE_FLAGS_HPP_