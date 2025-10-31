# TypeScript Library Usage

## Installation

```bash
npm install @webctp/ctp
```

## Import

```typescript
import * as CTP from "@webctp/ctp";
import * as Flags from "@webctp/ctp/flags";
```

## MarketData Class

### Constructor

```typescript
const md = new CTP.MarketData(brokerID?: string, userID?: string);
```

### Methods

| Method | Description | Parameters |
|---------|-------------|------------|
| `setFront(addr, port)` | Set front address and port | `addr` (string): Front address<br>`port` (string): Front port |
| `connect(addr, port)` | Connect to WebSocket server | `addr` (string): Server address<br>`port` (string): Server port |
| `login(password)` | Login | `password` (string): Password |
| `logout()` | Logout | None |
| `subscribe(instruments)` | Subscribe market data | `instruments` (string[]): Instrument code array |
| `unsubscribe(instruments)` | Unsubscribe market data | `instruments` (string[]): Instrument code array |
| `getTradingDay()` | Get trading day | None |
| `setBrokerID(brokerID)` | Set broker ID | `brokerID` (string): Broker ID |
| `setUserID(userID)` | Set user ID | `userID` (string): User ID |

### Callback Functions

| Callback | Trigger Condition | Parameter Description |
|----------|------------------|----------------------|
| `onInit` | When receiving server ready message | `data`: Message object containing `status: "ready"` |
| `onPerformed` | When receiving `PERFORMED` (0) message | `data.err`: Error info<br>`data.info`: Contains `req_id`, `code`, etc. |
| `onError` | When receiving `ERROR` (1) message | `data.err`: Error info (`code`, `msg`)<br>`data.info`: Contains `req_id`, `is_last`, etc. |
| `onConnected` | When receiving `CONNECTED` (2) message | `data`: Connection success message object |
| `onDisconnected` | When receiving `DISCONNECTED` (3) message | `data.info`: Contains `reason` (disconnect reason code) |
| `onHeartbeatTimeout` | When receiving `HEARTBEAT_TIMEOUT` (4) message | `data.info`: Contains `time` (timeout time) |
| `onLogin` | When receiving `LOGIN` (5) message | `data.err`: Error info<br>`data.info`: Contains `trading_day`, `login_time`, `broker_id`, `user_id`, etc. |
| `onLogout` | When receiving `LOGOUT` (6) message | `data.err`: Error info<br>`data.info`: Contains `broker_id`, `user_id`, `req_id`, `is_last` |
| `onTradingDay` | When receiving `TRADING_DAY` (7) message | `data.info`: Contains `trading_day` |
| `onSubscribe` | When receiving `SUBSCRIBE` (8) message | `data.err`: Error info<br>`data.info`: Contains `instrument_id`, `req_id`, `is_last` |
| `onUnsubscribe` | When receiving `UNSUBSCRIBE` (9) message | `data.err`: Error info<br>`data.info`: Contains `instrument_id`, `req_id`, `is_last` |
| `onMarketData` | When receiving `MARKET_DATA` (10) message | `data.info`: Contains complete market data including `trading_day`, `instrument_id`, `exchange_id`, `exchange_inst_id`, `last_price`, `pre_settlement_price`, `pre_close_price`, `pre_open_interest`, `open_price`, `highest_price`, `lowest_price`, `volume`, `turnover`, `open_interest`, `close_price`, `settlement_price`, `upper_limit_price`, `lower_limit_price`, `pre_delta`, `curr_delta`, `update_time`, `update_millisec`, `bp1`-`bp5` (bid prices 1-5), `bv1`-`bv5` (bid volumes 1-5), `ap1`-`ap5` (ask prices 1-5), `av1`-`av5` (ask volumes 1-5), `average_price`, `action_day`, `banding_upper_price`, `banding_lower_price`, etc. |

### Usage Example

```typescript
const md = new CTP.MarketData("9999", "248361");
md.setFront("182.254.243.31", "30001");

md.onInit = (data) => {
    console.log("Server ready");
};

md.onConnected = (data) => {
    console.log("Connected to CTP");
    md.login("password123");
};

md.onLogin = (data) => {
    console.log("Logged in, trading day:", data.info.trading_day);
    md.subscribe(["ag2512", "cu2512"]);
};

md.onMarketData = (data) => {
    console.log("Market data:", data.info.instrument_id, data.info.last_price);
};

md.connect("localhost", "8888");
```

## Trade Class

### Constructor

```typescript
const trade = new CTP.Trade(brokerID?: string, investorID?: string);
```

### Methods

| Method | Description | Parameters |
|---------|-------------|------------|
| `setFront(addr, port)` | Set front address and port | `addr` (string): Front address<br>`port` (string): Front port |
| `connect(addr, port)` | Connect to WebSocket server | `addr` (string): Server address<br>`port` (string): Server port |
| `set(brokerID?, investorID?)` | Set broker and investor ID | `brokerID` (string, optional): Broker ID<br>`investorID` (string, optional): Investor ID |
| `auth(userID, appID, authCode)` | Client authentication | `userID` (string): User ID<br>`appID` (string): Application ID<br>`authCode` (string): Auth code |
| `login(userID, password)` | Login | `userID` (string): User ID<br>`password` (string): Password |
| `logout(userID)` | Logout | `userID` (string): User ID |
| `getTradingDay()` | Get trading day | None |
| `querySettlementInfo(tradingDay)` | Query settlement info | `tradingDay` (string): Trading day |
| `confirmSettlementInfo()` | Confirm settlement info | None |
| `queryTradingAccount()` | Query trading account | None |
| `insertOrder(...)` | Insert order | `instrument` (string): Instrument code<br>`exchange` (string): Exchange code<br>`ref` (string): Order reference<br>`price` (number): Price<br>`direction` (number): Direction<br>`offset` (number): Offset flag<br>`volume` (number): Volume<br>`priceType` (number): Price type<br>`timeCondition` (number): Time condition |
| `queryOrder(options?)` | Query order | `options` (optional): `{orderSysID?, exchangeID?, from?, to?}` |

### Callback Functions

| Callback | Trigger Condition | Parameter Description |
|----------|------------------|----------------------|
| `onInit` | When receiving server ready message | `data`: Message object containing `status: "ready"` |
| `onPerformed` | When receiving `PERFORMED` (0) message | `data.err`: Error info<br>`data.info`: Contains `req_id`, `code`, etc. |
| `onError` | When receiving `ERROR` (1) message | `data.err`: Error info (`code`, `msg`)<br>`data.info`: Contains `req_id`, `is_last`, etc. |
| `onErrorNull` | When receiving `ERROR_NULL` (2) message | `data`: NULL pointer error message object |
| `onErrorUnknownValue` | When receiving `ERROR_UNKNOWN_VALUE` (3) message | `data.info`: Contains error info string |
| `onConnected` | When receiving `CONNECTED` (4) message | `data`: Connection success message object |
| `onTradingDay` | When receiving `TRADING_DAY` (5) message | `data.info`: Contains `trading_day` |
| `onDisconnected` | When receiving `DISCONNECTED` (6) message | `data.err`: Contains `code` (disconnect reason code), `msg` (disconnect reason description) |
| `onAuthenticate` | When receiving `AUTHENTICATE` (7) message | `data.err`: Error info<br>`data.info`: Contains `req_id`, `is_last`, `app_id`, `app_type`, `broker_id`, `user_id`, `user_product_info` |
| `onLogin` | When receiving `LOGIN` (8) message | `data.err`: Error info<br>`data.info`: Contains `trading_day`, `login_time`, `broker_id`, `user_id`, `front_id`, `session_id`, etc. |
| `onLogout` | When receiving `LOGOUT` (9) message | `data.err`: Error info<br>`data.info`: Contains `broker_id`, `user_id`, `req_id`, `is_last` |
| `onSettlementInfo` | When receiving `SETTLEMENT_INFO` (10) message | `data.err`: Error info<br>`data.info`: Contains `trading_day`, `settlement_id`, `broker_id`, `investor_id`, `sequence_no`, `content`, `account_id`, `currency_id`, `req_id`, `is_last` |
| `onSettlementInfoConfirm` | When receiving `SETTLEMENT_INFO_CONFIRM` (11) message | `data.err`: Error info<br>`data.info`: Contains `broker_id`, `investor_id`, `confirm_date`, `confirm_time`, `settlement_id`, `account_id`, `currency_id`, `req_id`, `is_last` |
| `onTradingAccount` | When receiving `TRADING_ACCOUNT` (12) message | `data.err`: Error info<br>`data.info`: Contains account details including `broker_id`, `account_id`, `pre_mortgage`, `pre_credit`, `pre_deposit`, `pre_balance` (pre-settlement reserve), `pre_margin`, `interest_base`, `interest`, `deposit`, `withdraw`, `frozen_margin`, `frozen_cash`, `frozen_commission`, `current_margin`, `cash_in`, `commission`, `close_profit`, `position_profit`, `balance` (futures settlement reserve), `available`, `withdraw_quota`, `reserve`, `trading_day`, `settlement_id`, `credit`, `mortgage`, `exchange_margin`, `delivery_margin`, `exchange_delivery_margin`, `reserve_balance`, `currency_id`, etc. |
| `onOrderInsertError` | When receiving `ORDER_INSERT_ERROR` (13) message | `data.err`: Error info (`code`, `msg`)<br>`data.info`: Contains `req_id`, `is_last` |
| `onOrderInserted` | When receiving `ORDER_INSERTED` (14) message | `data.err`: Error info<br>`data.info`: Contains `broker_id`, `investor_id`, `user_id`, `exchange_id`, `req_id`, `ref`, `order_local_id`, `order_sys_id`, `instrument_id`, `insert_date`, `insert_time`, `order_submit_status`, `order_status`, `volume_traded`, `volume_total`, `status_msg` |
| `onOrderTraded` | When receiving `ORDER_TRADED` (15) message | `data.err`: Error info<br>`data.info`: Contains `broker_id`, `investor_id`, `user_id`, `ref`, `exchange_id`, `trade_id`, `order_sys_id`, `order_local_id`, `broker_order_seq`, `settlement_id`, `volume` |
| `onQueryOrder` | When receiving `QUERY_ORDER` (16) message | `data.err`: Error info<br>`data.info`: Contains order details including `broker_id`, `investor_id`, `user_id`, `exchange_id`, `req_id`, `ref`, `order_local_id`, `order_sys_id`, `instrument_id`, `insert_date`, `insert_time`, `order_submit_status`, `order_status`, `volume_traded`, `volume_total`, `status_msg`, `is_last` |

### Usage Example

```typescript
const trade = new CTP.Trade("9999", "248361");
trade.setFront("182.254.243.31", "30001");

trade.onInit = (data) => {
    console.log("Server ready");
};

trade.onConnected = (data) => {
    console.log("Connected to CTP");
    trade.set("9999", "248361");
    trade.auth("248361", "simnow_client_test", "0000000000000000");
};

trade.onAuthenticate = (data) => {
    console.log("Authenticated");
    trade.login("248361", "password123");
};

trade.onLogin = (data) => {
    console.log("Logged in, trading day:", data.info.trading_day);
    trade.querySettlementInfo("20251031");
};

trade.onSettlementInfo = (data) => {
    console.log("Settlement info:", data.info.content);
    if (data.info.is_last) {
        trade.confirmSettlementInfo();
    }
};

trade.onSettlementInfoConfirm = (data) => {
    console.log("Settlement confirmed");
    trade.queryTradingAccount();
};

trade.onTradingAccount = (data) => {
    console.log("Available:", data.info.available);
    // Insert order example
    trade.insertOrder(
        "ag2512",
        "SHFE",
        "114514",
        11000,
        Flags.Direction.BUY,
        Flags.OrderOffset.OPEN,
        1,
        Flags.OrderPriceType.LIMITED,
        Flags.TimeCondition.ONE_DAY
    );
};

trade.onOrderInserted = (data) => {
    console.log("Order inserted:", data.info.order_sys_id);
};

trade.onOrderTraded = (data) => {
    console.log("Order traded:", data.info.volume);
};

trade.connect("localhost", "8888");
```

## Flags Enums

### Direction (Buy/Sell Direction)

```typescript
Flags.Direction.BUY  // 0: Buy
Flags.Direction.SELL // 1: Sell
```

### OrderOffset (Open/Close Flag)

```typescript
Flags.OrderOffset.OPEN           // 0: Open
Flags.OrderOffset.CLOSE          // 1: Close
Flags.OrderOffset.FORCED_CLOSE   // 2: Forced close
Flags.OrderOffset.CLUSTER_TODAY  // 3: Close today
Flags.OrderOffset.CLOSE_YESTERDAY // 4: Close yesterday
Flags.OrderOffset.FORCED_OFF     // 5: Forced off
Flags.OrderOffset.LOCAL_FORCED_CLOSE // 6: Local forced close
```

### OrderPriceType (Order Price Type)

```typescript
Flags.OrderPriceType.LIMITED // 0: Limited
Flags.OrderPriceType.MARKET  // 1: Market
Flags.OrderPriceType.LAST    // 2: Best
```

### TimeCondition (Time Condition)

```typescript
Flags.TimeCondition.IMMEDIATE // 0: Immediate
Flags.TimeCondition.ONE_DAY   // 1: One day valid
```

