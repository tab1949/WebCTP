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
        throw std::runtime_error(std::string("tabxx::GetDirection(TThostFtdcDirectionType): Unknown order direction: ") + d);
    }
}

inline TThostFtdcDirectionType GetDirection(Direction d) {
    switch (d) {
    case Direction::BUY:
        return THOST_FTDC_D_Buy;
    case Direction::SELL:
        return THOST_FTDC_D_Sell;
    default:
        throw std::runtime_error(std::string("tabxx::GetDirection(tabxx::Direction): Unknown order direction: ") + static_cast<char>(d));
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
        throw std::runtime_error(std::string("tabxx::GetOrderOperation(TThostFtdcOffsetFlagType): Unknown order operation type: ") + o);
    }
}

inline TThostFtdcOffsetFlagType GetOrderOperation(OrderOffset of) {
    switch (of) {
    case OrderOffset::OPEN:
        return THOST_FTDC_OF_Open;
    case OrderOffset::CLOSE:
        return THOST_FTDC_OF_Close;
    case OrderOffset::CLOSE_TODAY:
        return THOST_FTDC_OF_CloseToday;
    case OrderOffset::CLOSE_YESTERDAY:
        return THOST_FTDC_OF_CloseYesterday;
    case OrderOffset::FORCED_CLOSE:
        return THOST_FTDC_OF_ForceClose;
    case OrderOffset::FORCED_OFF:
        return THOST_FTDC_OF_ForceOff;
    case OrderOffset::LOCAL_FORCED_CLOSE:
        return THOST_FTDC_OF_LocalForceClose;
    default: 
        throw std::runtime_error(std::string("tabxx::GetOrderOperation(tabxx::OrderOffset): Unknown order operation type: ") + static_cast<char>(of));
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
        throw std::runtime_error(std::string("tabxx::GetOrderPriceType(TThostFtdcOrderPriceTypeType): Unknown order price type: ") + o);
    }
}

inline TThostFtdcOrderPriceTypeType GetOrderPriceType(OrderPriceType opt) {
    switch (opt) {
    case OrderPriceType::LIMITED:
        return THOST_FTDC_OPT_LimitPrice;
    case OrderPriceType::MARKET:
        return THOST_FTDC_OPT_AnyPrice;
    case OrderPriceType::LAST:
        return THOST_FTDC_OPT_LastPrice;
    default:
        throw std::runtime_error(std::string("tabxx::GetOrderPriceType(tabxx::OrderPriceType): Unknown order price type: ") + static_cast<char>(opt));
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
        throw std::runtime_error((std::string)"tabxx::GetHedge(TThostFtdcHedgeFlagType): Unknown hedge type: " + h);
    }
}

inline TThostFtdcHedgeFlagType GetHedge(Hedge hedge) {
    switch (hedge) {
    case Hedge::SPECULATION:
        return THOST_FTDC_HF_Speculation;
    default:
        throw std::runtime_error((std::string)"tabxx::GetHedge(tabxx::Hedge): Unknown hedge type: " + static_cast<char>(hedge));
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
        throw std::runtime_error((std::string)"tabxx::GetTimeCondition(TThostFtdcTimeConditionType): Unknown time condition: " + tc);
    }
}

inline TThostFtdcTimeConditionType GetTimeCondition(TimeCondition tc) {
    switch (tc) {
    case TimeCondition::IMMEDIATE:
        return THOST_FTDC_TC_IOC;
    case TimeCondition::ONE_DAY:
        return THOST_FTDC_TC_GFD;
    default: 
        throw std::runtime_error((std::string)"tabxx::GetTimeCondition(tabxx::TimeCondition): Unknown time condition: " + static_cast<char>(tc));
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
        throw std::runtime_error((std::string)"tabxx::GetOrderSubmitStatus(TThostFtdcOrderSubmitStatusType): Unknown order submitting status: " + s);
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
        throw std::runtime_error((std::string)"tabxx::GetOrderStatus(TThostFtdcOrderStatusType): Unknown order status: " + s);
    }
}

inline std::string MakeOrderBrief(
    TThostFtdcOrderRefType ref, 
    TThostFtdcInstrumentIDType instrument_id, 
    TThostFtdcExchangeIDType exchange_id,
    TThostFtdcVolumeType volume,
    TThostFtdcPriceType price,
    Direction direction, 
    OrderOffset of, 
    OrderPriceType opt, 
    Hedge hedge, 
    TimeCondition tc) {
    std::string ret = "REF ";
    ret += ref;
    ret += ',';
    ret += hedge == Hedge::SPECULATION ? "SPEC " : "HEDGE ";
    ret += direction == Direction::BUY ? "BUY " : "SELL ";
    switch (of) {
    case OrderOffset::OPEN:
        ret += "OPEN ";
        break;
    case OrderOffset::CLOSE:
        ret += "CLOSE ";
        break;
    case OrderOffset::CLOSE_TODAY:
        ret += "CLOSE_TODAY ";
        break;
    case OrderOffset::CLOSE_YESTERDAY:
        ret += "CLOSE_YESTERDAY ";
        break;
    case OrderOffset::FORCED_CLOSE:
        ret += "FORCED_CLOSE ";
        break;
    case OrderOffset::FORCED_OFF:
        ret += "FORCED_OFF ";
        break;
    case OrderOffset::LOCAL_FORCED_CLOSE:
        ret += "LOCAL_FORCED_CLOSE ";
        break;
    }
    ret += std::to_string(volume);
    ret += ' ';
    ret += instrument_id;
    ret += '@';
    ret += exchange_id;
    ret += ',';
    switch (opt) {
    case OrderPriceType::LIMITED:
        ret += "LIMITED_PRICE";
        break;
    case OrderPriceType::MARKET:
        ret += "MARKET_PRICE";
        break;
    case OrderPriceType::LAST:
        ret += "LAST_PRICE";
        break;
    }
    ret += '@';
    ret += std::to_string(price);
    ret += " ";
    ret += tc == TimeCondition::IMMEDIATE ? "IMMEDIATELY" : "ONE_DAY";
    return ret;
}

} // namespace tabxx

#endif // TABXX_TRADE_FLAGS_HPP_