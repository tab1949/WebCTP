# TypeScript 库使用方法

## 安装

```bash
npm install @webctp/ctp
```

## 导入

```typescript
import * as CTP from "@webctp/ctp";
import * as Flags from "@webctp/ctp/flags";
```

## MarketData 类

### 构造函数

```typescript
const md = new CTP.MarketData(brokerID?: string, userID?: string);
```

### 方法

| 方法 | 说明 | 参数 |
|------|------|------|
| `setFront(addr, port)` | 设置前置地址和端口 | `addr` (string): 前置地址<br>`port` (string): 前置端口 |
| `connect(addr, port)` | 连接到WebSocket服务器 | `addr` (string): 服务器地址<br>`port` (string): 服务器端口 |
| `login(password)` | 登录 | `password` (string): 密码 |
| `logout()` | 登出 | 无 |
| `subscribe(instruments)` | 订阅行情 | `instruments` (string[]): 合约代码数组 |
| `unsubscribe(instruments)` | 取消订阅 | `instruments` (string[]): 合约代码数组 |
| `getTradingDay()` | 获取交易日 | 无 |
| `setBrokerID(brokerID)` | 设置经纪商代码 | `brokerID` (string): 经纪商代码 |
| `setUserID(userID)` | 设置用户代码 | `userID` (string): 用户代码 |

### 回调函数

| 回调函数 | 触发条件 | 参数说明 |
|----------|----------|----------|
| `onInit` | 收到服务器 ready 消息时 | `data`: 包含 `status: "ready"` 的消息对象 |
| `onPerformed` | 收到 `PERFORMED` (0) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `req_id`, `code` 等信息 |
| `onError` | 收到 `ERROR` (1) 消息时 | `data.err`: 错误信息 (`code`, `msg`)<br>`data.info`: 包含 `req_id`, `is_last` 等信息 |
| `onConnected` | 收到 `CONNECTED` (2) 消息时 | `data`: 连接成功消息对象 |
| `onDisconnected` | 收到 `DISCONNECTED` (3) 消息时 | `data.info`: 包含 `reason` (断开原因代码) |
| `onHeartbeatTimeout` | 收到 `HEARTBEAT_TIMEOUT` (4) 消息时 | `data.info`: 包含 `time` (超时时间) |
| `onLogin` | 收到 `LOGIN` (5) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `trading_day`, `login_time`, `broker_id`, `user_id` 等登录信息 |
| `onLogout` | 收到 `LOGOUT` (6) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `broker_id`, `user_id`, `req_id`, `is_last` |
| `onTradingDay` | 收到 `TRADING_DAY` (7) 消息时 | `data.info`: 包含 `trading_day` |
| `onSubscribe` | 收到 `SUBSCRIBE` (8) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `instrument_id`, `req_id`, `is_last` |
| `onUnsubscribe` | 收到 `UNSUBSCRIBE` (9) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `instrument_id`, `req_id`, `is_last` |
| `onMarketData` | 收到 `MARKET_DATA` (10) 消息时 | `data.info`: 包含完整的行情数据，包括 `trading_day`, `instrument_id`, `exchange_id`, `exchange_inst_id`, `last_price`, `pre_settlement_price`, `pre_close_price`, `pre_open_interest`, `open_price`, `highest_price`, `lowest_price`, `volume`, `turnover`, `open_interest`, `close_price`, `settlement_price`, `upper_limit_price`, `lower_limit_price`, `pre_delta`, `curr_delta`, `update_time`, `update_millisec`, `bp1`-`bp5` (申买价一到五), `bv1`-`bv5` (申买量一到五), `ap1`-`ap5` (申卖价一到五), `av1`-`av5` (申卖量一到五), `average_price`, `action_day`, `banding_upper_price`, `banding_lower_price` 等 |

### 使用示例

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

## Trade 类

### 构造函数

```typescript
const trade = new CTP.Trade(brokerID?: string, investorID?: string);
```

### 方法

| 方法 | 说明 | 参数 |
|------|------|------|
| `setFront(addr, port)` | 设置前置地址和端口 | `addr` (string): 前置地址<br>`port` (string): 前置端口 |
| `connect(addr, port)` | 连接到WebSocket服务器 | `addr` (string): 服务器地址<br>`port` (string): 服务器端口 |
| `set(brokerID?, investorID?)` | 设置经纪商和投资者代码 | `brokerID` (string, 可选): 经纪商代码<br>`investorID` (string, 可选): 投资者代码 |
| `auth(userID, appID, authCode)` | 客户端认证 | `userID` (string): 用户代码<br>`appID` (string): 应用标识<br>`authCode` (string): 认证码 |
| `login(userID, password)` | 登录 | `userID` (string): 用户代码<br>`password` (string): 密码 |
| `logout(userID)` | 登出 | `userID` (string): 用户代码 |
| `getTradingDay()` | 获取交易日 | 无 |
| `querySettlementInfo(tradingDay)` | 查询结算信息 | `tradingDay` (string): 交易日 |
| `confirmSettlementInfo()` | 确认结算信息 | 无 |
| `queryTradingAccount()` | 查询交易账户 | 无 |
| `insertOrder(...)` | 下单 | `instrument` (string): 合约代码<br>`exchange` (string): 交易所代码<br>`ref` (string): 报单引用<br>`price` (number): 价格<br>`direction` (number): 买卖方向<br>`offset` (number): 开平标志<br>`volume` (number): 数量<br>`priceType` (number): 报单价格类型<br>`timeCondition` (number): 有效期类型 |
| `queryOrder(options?)` | 查询报单 | `options` (可选): `{orderSysID?, exchangeID?, from?, to?}` |

### 回调函数

| 回调函数 | 触发条件 | 参数说明 |
|----------|----------|----------|
| `onInit` | 收到服务器 ready 消息时 | `data`: 包含 `status: "ready"` 的消息对象 |
| `onPerformed` | 收到 `PERFORMED` (0) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `req_id`, `code` 等信息 |
| `onError` | 收到 `ERROR` (1) 消息时 | `data.err`: 错误信息 (`code`, `msg`)<br>`data.info`: 包含 `req_id`, `is_last` 等信息 |
| `onErrorNull` | 收到 `ERROR_NULL` (2) 消息时 | `data`: NULL指针错误消息对象 |
| `onErrorUnknownValue` | 收到 `ERROR_UNKNOWN_VALUE` (3) 消息时 | `data.info`: 包含错误信息字符串 |
| `onConnected` | 收到 `CONNECTED` (4) 消息时 | `data`: 连接成功消息对象 |
| `onTradingDay` | 收到 `TRADING_DAY` (5) 消息时 | `data.info`: 包含 `trading_day` |
| `onDisconnected` | 收到 `DISCONNECTED` (6) 消息时 | `data.err`: 包含 `code` (断开原因代码), `msg` (断开原因描述) |
| `onAuthenticate` | 收到 `AUTHENTICATE` (7) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `req_id`, `is_last`, `app_id`, `app_type`, `broker_id`, `user_id`, `user_product_info` |
| `onLogin` | 收到 `LOGIN` (8) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `trading_day`, `login_time`, `broker_id`, `user_id`, `front_id`, `session_id` 等登录信息 |
| `onLogout` | 收到 `LOGOUT` (9) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `broker_id`, `user_id`, `req_id`, `is_last` |
| `onSettlementInfo` | 收到 `SETTLEMENT_INFO` (10) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `trading_day`, `settlement_id`, `broker_id`, `investor_id`, `sequence_no`, `content`, `account_id`, `currency_id`, `req_id`, `is_last` |
| `onSettlementInfoConfirm` | 收到 `SETTLEMENT_INFO_CONFIRM` (11) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `broker_id`, `investor_id`, `confirm_date`, `confirm_time`, `settlement_id`, `account_id`, `currency_id`, `req_id`, `is_last` |
| `onTradingAccount` | 收到 `TRADING_ACCOUNT` (12) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含账户详细信息，包括 `broker_id`, `account_id`, `pre_mortgage`, `pre_credit`, `pre_deposit`, `pre_balance` (上次结算准备金), `pre_margin`, `interest_base`, `interest`, `deposit`, `withdraw`, `frozen_margin`, `frozen_cash`, `frozen_commission`, `current_margin`, `cash_in`, `commission`, `close_profit`, `position_profit`, `balance` (期货结算准备金), `available`, `withdraw_quota`, `reserve`, `trading_day`, `settlement_id`, `credit`, `mortgage`, `exchange_margin`, `delivery_margin`, `exchange_delivery_margin`, `reserve_balance`, `currency_id` 等 |
| `onOrderInsertError` | 收到 `ORDER_INSERT_ERROR` (13) 消息时 | `data.err`: 错误信息 (`code`, `msg`)<br>`data.info`: 包含 `req_id`, `is_last` |
| `onOrderInserted` | 收到 `ORDER_INSERTED` (14) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `broker_id`, `investor_id`, `user_id`, `exchange_id`, `req_id`, `ref`, `order_local_id`, `order_sys_id`, `instrument_id`, `insert_date`, `insert_time`, `order_submit_status`, `order_status`, `volume_traded`, `volume_total`, `status_msg` |
| `onOrderTraded` | 收到 `ORDER_TRADED` (15) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含 `broker_id`, `investor_id`, `user_id`, `ref`, `exchange_id`, `trade_id`, `order_sys_id`, `order_local_id`, `broker_order_seq`, `settlement_id`, `volume` |
| `onQueryOrder` | 收到 `QUERY_ORDER` (16) 消息时 | `data.err`: 错误信息<br>`data.info`: 包含报单详细信息，包括 `broker_id`, `investor_id`, `user_id`, `exchange_id`, `req_id`, `ref`, `order_local_id`, `order_sys_id`, `instrument_id`, `insert_date`, `insert_time`, `order_submit_status`, `order_status`, `volume_traded`, `volume_total`, `status_msg`, `is_last` |

### 使用示例

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
    // 下单示例
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

## Flags 枚举

### Direction (买卖方向)

```typescript
Flags.Direction.BUY  // 0: 买
Flags.Direction.SELL // 1: 卖
```

### OrderOffset (开平标志)

```typescript
Flags.OrderOffset.OPEN           // 0: 开仓
Flags.OrderOffset.CLOSE          // 1: 平仓
Flags.OrderOffset.FORCED_CLOSE   // 2: 强平
Flags.OrderOffset.CLOSE_TODAY    // 3: 平今
Flags.OrderOffset.CLOSE_YESTERDAY // 4: 平昨
Flags.OrderOffset.FORCED_OFF     // 5: 强减
Flags.OrderOffset.LOCAL_FORCED_CLOSE // 6: 本地强平
```

### OrderPriceType (报单价格类型)

```typescript
Flags.OrderPriceType.LIMITED // 0: 限价
Flags.OrderPriceType.MARKET  // 1: 市价
Flags.OrderPriceType.LAST    // 2: 最优价
```

### TimeCondition (有效期类型)

```typescript
Flags.TimeCondition.IMMEDIATE // 0: 立即
Flags.TimeCondition.ONE_DAY   // 1: 当日有效
```

