#ifndef TABXX_TRADE_FLAGS_HPP_
#define TABXX_TRADE_FLAGS_HPP_

#include <ThostFtdcUserApiDataType.h>

namespace tabxx {

enum class Direction {
    BUY = 0, 
    SELL = 1
};

enum class OrderOffset {
    OPEN = 0, 
    CLOSE = 1,
    FORCED_CLOSE = 2,
    CLOSE_TODAY = 3,
    CLOSE_YESTERDAY = 4,
    FORCED_OFF = 5,
    LOCAL_FORCED_CLOSE = 6 
};

enum class OrderPriceType {
    LIMITED = 0, 
    MARKET = 1,
    LAST = 2
};

enum class Hedge {
    SPECULATION = 0,
};

enum class TimeCondition {
    IMMEDIATE = 0,
    ONE_DAY = 1
};

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
    default: throw s;
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
    default: throw s;
    }
}

} // namespace tabxx

#endif // TABXX_TRADE_FLAGS_HPP_