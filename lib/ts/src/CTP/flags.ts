export enum BizType {
    FUTURES = 49, STOCK = 50
};

export enum Hedge {
    SPECULATION = 0,
}

export enum TimeCondition {
    IMMEDIATE = 0,
    ONE_DAY = 1
}

export enum Direction {
    BUY = 0,
    SELL = 1
}

export enum OrderOffset {
    OPEN = 0,
    CLOSE = 1,
    FORCED_CLOSE = 2,
    CLOSE_TODAY = 3,
    CLOSE_YESTERDAY = 4,
    FORCED_OFF = 5,
    LOCAL_FORCED_CLOSE = 6
}

export enum OrderPriceType {
    LIMITED = 0,
    MARKET = 1,
    LAST = 2
}

export enum OrderSubmitStatus {
    INSERT_SUBMITTED = 0,
    CANCEL_SUBMITTED = 1,
    MODIFY_SUBMITTED = 2,
    ACCEPTED = 3,
    INSERT_REJECTED = 4,
    CANCEL_REJECTED = 5,
    MODIFY_REJECTED = 6
}

export enum OrderStatus {
    ALL_TRADED = 0,
    PART_TRADED_QUEUEING = 1,
    PART_TRADED_NOT_QUEUEING = 2,
    NOT_TRADED_QUEUEING = 3,
    NOT_TRADED_NOT_QUEUEING = 4,
    CANCELED = 5,
    UNKNOWN = 6,
    NOT_TOUCHED = 7,
    TOUCHED = 8
}