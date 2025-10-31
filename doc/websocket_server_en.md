# WebSocket Server API

WebCTP provides two WebSocket endpoints:
- `/market_data` - Market data interface
- `/trade` - Trading interface

## Message Format

All messages are in JSON format. Request message format:

```json
{
  "op": "operation_name",
  "data": {
    // operation parameters
  }
}
```

Response message format:

```json
{
  "msg": message_code,
  "err": {
    "code": error_code,
    "msg": "error_message"
  },
  "info": {
    // response data
  }
}
```

## Market Data Interface (`/market_data`)

### Operations

| Operation | Description | Request Parameters | Response Messages |
|-----------|------------|-------------------|-------------------|
| `connect` | Connect to CTP market data front | `addr` (string): Front address<br>`port` (string): Front port | `PERFORMED` (0) |
| `login` | Login | `broker_id` (string): Broker ID<br>`user_id` (string): User ID<br>`password` (string): Password | `PERFORMED` (0), `LOGIN` (5) |
| `logout` | Logout | `broker_id` (string): Broker ID<br>`user_id` (string): User ID | `PERFORMED` (0), `LOGOUT` (6) |
| `subscribe` | Subscribe market data | `instruments` (array): Instrument code array | `PERFORMED` (0), `SUBSCRIBE` (8) |
| `unsubscribe` | Unsubscribe market data | `instruments` (array): Instrument code array | `PERFORMED` (0), `UNSUBSCRIBE` (9) |
| `get_trading_day` | Get trading day | None | `TRADING_DAY` (7) |

### Response Messages

| Message Code | Message Name | Description | info Fields |
|--------------|--------------|-------------|-------------|
| 0 | `PERFORMED` | Operation completed | `req_id`: Request ID, `code`: Error code |
| 1 | `ERROR` | Error response | `req_id`: Request ID, `is_last`: Is last message |
| 2 | `CONNECTED` | Connection successful | None |
| 3 | `DISCONNECTED` | Disconnected | `reason`: Disconnect reason code |
| 4 | `HEARTBEAT_TIMEOUT` | Heartbeat timeout | `time`: Timeout time |
| 5 | `LOGIN` | Login response | `trading_day`: Trading day<br>`login_time`: Login time<br>`broker_id`: Broker ID<br>`user_id`: User ID<br>`front_id`: Front ID<br>`session_id`: Session ID<br>`max_order_ref`: Max order reference<br>And other login info fields |
| 6 | `LOGOUT` | Logout response | `broker_id`: Broker ID<br>`user_id`: User ID<br>`req_id`: Request ID<br>`is_last`: Is last message |
| 7 | `TRADING_DAY` | Trading day | `trading_day`: Trading day string |
| 8 | `SUBSCRIBE` | Subscribe response | `instrument_id`: Instrument code<br>`req_id`: Request ID<br>`is_last`: Is last message |
| 9 | `UNSUBSCRIBE` | Unsubscribe response | `instrument_id`: Instrument code<br>`req_id`: Request ID<br>`is_last`: Is last message |
| 10 | `MARKET_DATA` | Market data push | `trading_day`: Trading day<br>`instrument_id`: Instrument code<br>`exchange_id`: Exchange code<br>`exchange_inst_id`: Exchange instrument code<br>`last_price`: Last price<br>`pre_settlement_price`: Pre-settlement price<br>`pre_close_price`: Pre-close price<br>`pre_open_interest`: Pre-open interest<br>`open_price`: Open price<br>`highest_price`: Highest price<br>`lowest_price`: Lowest price<br>`volume`: Volume<br>`turnover`: Turnover<br>`open_interest`: Open interest<br>`close_price`: Close price<br>`settlement_price`: Settlement price<br>`upper_limit_price`: Upper limit price<br>`lower_limit_price`: Lower limit price<br>`pre_delta`: Pre-delta<br>`curr_delta`: Current delta<br>`update_time`: Last update time<br>`update_millisec`: Last update millisecond<br>`bp1`-`bp5`: Bid price 1-5<br>`bv1`-`bv5`: Bid volume 1-5<br>`ap1`-`ap5`: Ask price 1-5<br>`av1`-`av5`: Ask volume 1-5<br>`average_price`: Average price<br>`action_day`: Action day<br>`banding_upper_price`: Banding upper price<br>`banding_lower_price`: Banding lower price |

## Trading Interface (`/trade`)

### Operations

| Operation | Description | Request Parameters | Response Messages |
|-----------|------------|-------------------|-------------------|
| `connect` | Connect to CTP trading front | `addr` (string): Front address<br>`port` (string): Front port | `PERFORMED` (0), `CONNECTED` (4) |
| `set` | Set broker and investor ID | `broker_id` (string, optional): Broker ID<br>`investor_id` (string, optional): Investor ID | `PERFORMED` (0) |
| `auth` | Client authentication | `user_id` (string): User ID<br>`app_id` (string): Application ID<br>`auth_code` (string): Auth code | `PERFORMED` (0), `AUTHENTICATE` (7) |
| `login` | Login | `user_id` (string): User ID<br>`password` (string): Password | `PERFORMED` (0), `LOGIN` (8) |
| `logout` | Logout | `user_id` (string): User ID | `PERFORMED` (0), `LOGOUT` (9) |
| `get_trading_day` | Get trading day | None | `TRADING_DAY` (5) |
| `query_settlement_info` | Query settlement info | `trading_day` (string): Trading day | `PERFORMED` (0), `SETTLEMENT_INFO` (10) |
| `confirm_settlement_info` | Confirm settlement info | None | `PERFORMED` (0), `SETTLEMENT_INFO_CONFIRM` (11) |
| `query_trading_account` | Query trading account | None | `PERFORMED` (0), `TRADING_ACCOUNT` (12) |
| `insert_order` | Insert order | `instrument` (string): Instrument code<br>`exchange` (string): Exchange code<br>`ref` (string): Order reference<br>`price` (number): Price<br>`direction` (number): Direction (0:Buy, 1:Sell)<br>`offset` (number): Offset flag (0:Open, 1:Close, ...)<br>`volume` (number): Volume<br>`price_type` (number): Price type (0:Limited, 1:Market, 2:Best)<br>`time_condition` (number): Time condition (0:Immediate, 1:One day) | `PERFORMED` (0), `ORDER_INSERT_ERROR` (13), `ORDER_INSERTED` (14) |
| `query_order` | Query order | `order_sys_id` (string, optional): System order ID<br>`exchange_id` (string, optional): Exchange code<br>`from` (string, optional): Start date<br>`to` (string, optional): End date<br>Or empty to query all | `PERFORMED` (0), `QUERY_ORDER` (16) |

### Response Messages

| Message Code | Message Name | Description | info Fields |
|--------------|--------------|-------------|-------------|
| 0 | `PERFORMED` | Operation completed | `req_id`: Request ID, `code`: Error code |
| 1 | `ERROR` | Error response | `req_id`: Request ID, `is_last`: Is last message |
| 2 | `ERROR_NULL` | NULL pointer error | `req_id`: Request ID |
| 3 | `ERROR_UNKNOWN_VALUE` | Unknown value error | `info`: Error info |
| 4 | `CONNECTED` | Connection successful | None |
| 5 | `TRADING_DAY` | Trading day | `trading_day`: Trading day string |
| 6 | `DISCONNECTED` | Disconnected | `code`: Disconnect reason code<br>`msg`: Disconnect reason description |
| 7 | `AUTHENTICATE` | Authentication response | `req_id`: Request ID<br>`is_last`: Is last message<br>`app_id`: Application ID<br>`app_type`: Application type<br>`broker_id`: Broker ID<br>`user_id`: User ID<br>`user_product_info`: User product info |
| 8 | `LOGIN` | Login response | `req_id`: Request ID<br>`is_last`: Is last message<br>`trading_day`: Trading day<br>`login_time`: Login time<br>`broker_id`: Broker ID<br>`user_id`: User ID<br>`system_name`: System name<br>`front_id`: Front ID<br>`session_id`: Session ID<br>`max_order_ref`: Max order reference<br>And other login info fields |
| 9 | `LOGOUT` | Logout response | `req_id`: Request ID<br>`is_last`: Is last message<br>`broker_id`: Broker ID<br>`user_id`: User ID |
| 10 | `SETTLEMENT_INFO` | Settlement info | `req_id`: Request ID<br>`is_last`: Is last message<br>`trading_day`: Trading day<br>`settlement_id`: Settlement ID<br>`broker_id`: Broker ID<br>`investor_id`: Investor ID<br>`sequence_no`: Sequence number<br>`content`: Settlement content<br>`account_id`: Account ID<br>`currency_id`: Currency code |
| 11 | `SETTLEMENT_INFO_CONFIRM` | Settlement info confirm | `req_id`: Request ID<br>`is_last`: Is last message<br>`broker_id`: Broker ID<br>`investor_id`: Investor ID<br>`confirm_date`: Confirm date<br>`confirm_time`: Confirm time<br>`settlement_id`: Settlement ID<br>`account_id`: Account ID<br>`currency_id`: Currency code |
| 12 | `TRADING_ACCOUNT` | Trading account | `req_id`: Request ID<br>`is_last`: Is last message<br>`broker_id`: Broker ID<br>`account_id`: Account ID<br>`pre_mortgage`: Pre-mortgage<br>`pre_credit`: Pre-credit<br>`pre_deposit`: Pre-deposit<br>`pre_balance`: Pre-balance (pre-settlement reserve)<br>`pre_margin`: Pre-margin<br>`interest_base`: Interest base<br>`interest`: Interest<br>`deposit`: Deposit<br>`withdraw`: Withdraw<br>`frozen_margin`: Frozen margin<br>`frozen_cash`: Frozen cash<br>`frozen_commission`: Frozen commission<br>`current_margin`: Current margin<br>`cash_in`: Cash in<br>`commission`: Commission<br>`close_profit`: Close profit<br>`position_profit`: Position profit<br>`balance`: Balance (futures settlement reserve)<br>`available`: Available<br>`withdraw_quota`: Withdraw quota<br>`reserve`: Reserve<br>`trading_day`: Trading day<br>`settlement_id`: Settlement ID<br>`credit`: Credit<br>`mortgage`: Mortgage<br>`exchange_margin`: Exchange margin<br>`delivery_margin`: Delivery margin<br>`exchange_delivery_margin`: Exchange delivery margin<br>`reserve_balance`: Reserve balance<br>`currency_id`: Currency ID<br>And other account fields |
| 13 | `ORDER_INSERT_ERROR` | Order insert error | `req_id`: Request ID<br>`is_last`: Is last message |
| 14 | `ORDER_INSERTED` | Order inserted | `broker_id`: Broker ID<br>`investor_id`: Investor ID<br>`user_id`: User ID<br>`exchange_id`: Exchange code<br>`req_id`: Request ID<br>`ref`: Order reference<br>`order_local_id`: Local order ID<br>`order_sys_id`: System order ID<br>`instrument_id`: Instrument code<br>`insert_date`: Insert date<br>`insert_time`: Insert time<br>`order_submit_status`: Order submit status<br>`order_status`: Order status<br>`volume_traded`: Volume traded<br>`volume_total`: Volume total<br>`status_msg`: Status message |
| 15 | `ORDER_TRADED` | Order traded | `broker_id`: Broker ID<br>`investor_id`: Investor ID<br>`user_id`: User ID<br>`ref`: Order reference<br>`exchange_id`: Exchange code<br>`trade_id`: Trade ID<br>`order_sys_id`: System order ID<br>`order_local_id`: Local order ID<br>`broker_order_seq`: Broker order sequence<br>`settlement_id`: Settlement ID<br>`volume`: Volume |
| 16 | `QUERY_ORDER` | Query order response | `broker_id`: Broker ID<br>`investor_id`: Investor ID<br>`user_id`: User ID<br>`exchange_id`: Exchange code<br>`req_id`: Request ID<br>`ref`: Order reference<br>`order_local_id`: Local order ID<br>`order_sys_id`: System order ID<br>`instrument_id`: Instrument code<br>`insert_date`: Insert date<br>`insert_time`: Insert time<br>`order_submit_status`: Order submit status<br>`order_status`: Order status<br>`volume_traded`: Volume traded<br>`volume_total`: Volume total<br>`status_msg`: Status message<br>`is_last`: Is last message |

