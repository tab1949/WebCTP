# WebSocket Server 接口

WebCTP 提供了两个 WebSocket 端点：
- `/market_data` - 行情数据接口
- `/trade` - 交易接口

## 消息格式

所有消息均为 JSON 格式，请求消息格式如下：

```json
{
  "op": "操作名称",
  "data": {
    // 操作参数
  }
}
```

响应消息格式如下：

```json
{
  "msg": 消息代码,
  "err": {
    "code": 错误代码,
    "msg": "错误消息"
  },
  "info": {
    // 响应数据
  }
}
```

## 行情数据接口 (`/market_data`)

### 操作列表

| 操作 | 说明 | 请求参数 | 返回消息 |
|------|------|----------|----------|
| `connect` | 连接CTP行情前置 | `addr` (string): 前置地址<br>`port` (string): 前置端口 | `PERFORMED` (0) |
| `login` | 登录 | `broker_id` (string): 经纪商代码<br>`user_id` (string): 用户代码<br>`password` (string): 密码 | `PERFORMED` (0), `LOGIN` (5) |
| `logout` | 登出 | `broker_id` (string): 经纪商代码<br>`user_id` (string): 用户代码 | `PERFORMED` (0), `LOGOUT` (6) |
| `subscribe` | 订阅行情 | `instruments` (array): 合约代码数组 | `PERFORMED` (0), `SUBSCRIBE` (8) |
| `unsubscribe` | 取消订阅 | `instruments` (array): 合约代码数组 | `PERFORMED` (0), `UNSUBSCRIBE` (9) |
| `get_trading_day` | 获取交易日 | 无 | `TRADING_DAY` (7) |

### 返回消息列表

| 消息代码 | 消息名称 | 说明 | info 字段 |
|----------|----------|------|-----------|
| 0 | `PERFORMED` | 操作完成 | `req_id`: 请求ID, `code`: 错误代码 |
| 1 | `ERROR` | 错误响应 | `req_id`: 请求ID, `is_last`: 是否最后一条 |
| 2 | `CONNECTED` | 连接成功 | 无 |
| 3 | `DISCONNECTED` | 断开连接 | `reason`: 断开原因代码 |
| 4 | `HEARTBEAT_TIMEOUT` | 心跳超时 | `time`: 超时时间 |
| 5 | `LOGIN` | 登录响应 | `trading_day`: 交易日<br>`login_time`: 登录时间<br>`broker_id`: 经纪商代码<br>`user_id`: 用户代码<br>`front_id`: 前置编号<br>`session_id`: 会话编号<br>`max_order_ref`: 最大报单引用<br>以及其他登录信息字段 |
| 6 | `LOGOUT` | 登出响应 | `broker_id`: 经纪商代码<br>`user_id`: 用户代码<br>`req_id`: 请求ID<br>`is_last`: 是否最后一条 |
| 7 | `TRADING_DAY` | 交易日 | `trading_day`: 交易日字符串 |
| 8 | `SUBSCRIBE` | 订阅响应 | `instrument_id`: 合约代码<br>`req_id`: 请求ID<br>`is_last`: 是否最后一条 |
| 9 | `UNSUBSCRIBE` | 取消订阅响应 | `instrument_id`: 合约代码<br>`req_id`: 请求ID<br>`is_last`: 是否最后一条 |
| 10 | `MARKET_DATA` | 行情数据推送 | `trading_day`: 交易日<br>`instrument_id`: 合约代码<br>`exchange_id`: 交易所代码<br>`exchange_inst_id`: 合约在交易所的代码<br>`last_price`: 最新价<br>`pre_settlement_price`: 上次结算价<br>`pre_close_price`: 昨收盘<br>`pre_open_interest`: 昨持仓量<br>`open_price`: 今开盘<br>`highest_price`: 最高价<br>`lowest_price`: 最低价<br>`volume`: 数量<br>`turnover`: 成交金额<br>`open_interest`: 持仓量<br>`close_price`: 今收盘<br>`settlement_price`: 本次结算价<br>`upper_limit_price`: 涨停板价<br>`lower_limit_price`: 跌停板价<br>`pre_delta`: 昨虚实度<br>`curr_delta`: 今虚实度<br>`update_time`: 最后修改时间<br>`update_millisec`: 最后修改毫秒<br>`bp1`-`bp5`: 申买价一到申买价五<br>`bv1`-`bv5`: 申买量一到申买量五<br>`ap1`-`ap5`: 申卖价一到申卖价五<br>`av1`-`av5`: 申卖量一到申卖量五<br>`average_price`: 当日均价<br>`action_day`: 业务日期<br>`banding_upper_price`: 上带价<br>`banding_lower_price`: 下带价 |

## 交易接口 (`/trade`)

### 操作列表

| 操作 | 说明 | 请求参数 | 返回消息 |
|------|------|----------|----------|
| `connect` | 连接CTP交易前置 | `addr` (string): 前置地址<br>`port` (string): 前置端口 | `PERFORMED` (0), `CONNECTED` (4) |
| `set` | 设置经纪商和投资者代码 | `broker_id` (string, 可选): 经纪商代码<br>`investor_id` (string, 可选): 投资者代码 | `PERFORMED` (0) |
| `auth` | 客户端认证 | `user_id` (string): 用户代码<br>`app_id` (string): 应用标识<br>`auth_code` (string): 认证码 | `PERFORMED` (0), `AUTHENTICATE` (7) |
| `login` | 登录 | `user_id` (string): 用户代码<br>`password` (string): 密码 | `PERFORMED` (0), `LOGIN` (8) |
| `logout` | 登出 | `user_id` (string): 用户代码 | `PERFORMED` (0), `LOGOUT` (9) |
| `get_trading_day` | 获取交易日 | 无 | `TRADING_DAY` (5) |
| `query_settlement_info` | 查询结算信息 | `trading_day` (string): 交易日 | `PERFORMED` (0), `SETTLEMENT_INFO` (10) |
| `confirm_settlement_info` | 确认结算信息 | 无 | `PERFORMED` (0), `SETTLEMENT_INFO_CONFIRM` (11) |
| `query_trading_account` | 查询交易账户 | 无 | `PERFORMED` (0), `TRADING_ACCOUNT` (12) |
| `insert_order` | 下单 | `instrument` (string): 合约代码<br>`exchange` (string): 交易所代码<br>`ref` (string): 报单引用<br>`price` (number): 价格<br>`direction` (number): 买卖方向 (0:买, 1:卖)<br>`offset` (number): 开平标志 (0:开仓, 1:平仓, ...)<br>`volume` (number): 数量<br>`price_type` (number): 报单价格类型 (0:限价, 1:市价, 2:最优价)<br>`time_condition` (number): 有效期类型 (0:立即, 1:当日有效) | `PERFORMED` (0), `ORDER_INSERT_ERROR` (13), `ORDER_INSERTED` (14) |
| `query_order` | 查询报单 | `order_sys_id` (string, 可选): 系统报单编号<br>`exchange_id` (string, 可选): 交易所代码<br>`from` (string, 可选): 起始日期<br>`to` (string, 可选): 结束日期<br>或全部为空查询所有 | `PERFORMED` (0), `QUERY_ORDER` (16) |

### 返回消息列表

| 消息代码 | 消息名称 | 说明 | info 字段 |
|----------|----------|------|-----------|
| 0 | `PERFORMED` | 操作完成 | `req_id`: 请求ID, `code`: 错误代码 |
| 1 | `ERROR` | 错误响应 | `req_id`: 请求ID, `is_last`: 是否最后一条 |
| 2 | `ERROR_NULL` | NULL指针错误 | `req_id`: 请求ID |
| 3 | `ERROR_UNKNOWN_VALUE` | 未知值错误 | `info`: 错误信息 |
| 4 | `CONNECTED` | 连接成功 | 无 |
| 5 | `TRADING_DAY` | 交易日 | `trading_day`: 交易日字符串 |
| 6 | `DISCONNECTED` | 断开连接 | `code`: 断开原因代码<br>`msg`: 断开原因描述 |
| 7 | `AUTHENTICATE` | 认证响应 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`app_id`: 应用标识<br>`app_type`: 应用类型<br>`broker_id`: 经纪商代码<br>`user_id`: 用户代码<br>`user_product_info`: 用户产品信息 |
| 8 | `LOGIN` | 登录响应 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`trading_day`: 交易日<br>`login_time`: 登录时间<br>`broker_id`: 经纪商代码<br>`user_id`: 用户代码<br>`system_name`: 系统名称<br>`front_id`: 前置编号<br>`session_id`: 会话编号<br>`max_order_ref`: 最大报单引用<br>以及其他登录信息字段 |
| 9 | `LOGOUT` | 登出响应 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`broker_id`: 经纪商代码<br>`user_id`: 用户代码 |
| 10 | `SETTLEMENT_INFO` | 结算信息 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`trading_day`: 交易日<br>`settlement_id`: 结算编号<br>`broker_id`: 经纪商代码<br>`investor_id`: 投资者代码<br>`sequence_no`: 序号<br>`content`: 结算信息内容<br>`account_id`: 资金账号<br>`currency_id`: 币种代码 |
| 11 | `SETTLEMENT_INFO_CONFIRM` | 结算信息确认 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`broker_id`: 经纪商代码<br>`investor_id`: 投资者代码<br>`confirm_date`: 确认日期<br>`confirm_time`: 确认时间<br>`settlement_id`: 结算编号<br>`account_id`: 资金账号<br>`currency_id`: 币种代码 |
| 12 | `TRADING_ACCOUNT` | 交易账户 | `req_id`: 请求ID<br>`is_last`: 是否最后一条<br>`broker_id`: 经纪公司代码<br>`account_id`: 投资者帐号<br>`pre_mortgage`: 上次质押金额<br>`pre_credit`: 上次信用额度<br>`pre_deposit`: 上次存款额<br>`pre_balance`: 上次结算准备金<br>`pre_margin`: 上次占用的保证金<br>`interest_base`: 利息基数<br>`interest`: 利息收入<br>`deposit`: 入金金额<br>`withdraw`: 出金金额<br>`frozen_margin`: 冻结的保证金<br>`frozen_cash`: 冻结的资金<br>`frozen_commission`: 冻结的手续费<br>`current_margin`: 当前保证金总额<br>`cash_in`: 资金差额<br>`commission`: 手续费<br>`close_profit`: 平仓盈亏<br>`position_profit`: 持仓盈亏<br>`balance`: 期货结算准备金<br>`available`: 可用资金<br>`withdraw_quota`: 可取资金<br>`reserve`: 基本准备金<br>`trading_day`: 交易日<br>`settlement_id`: 结算编号<br>`credit`: 信用额度<br>`mortgage`: 质押金额<br>`exchange_margin`: 交易所保证金<br>`delivery_margin`: 投资者交割保证金<br>`exchange_delivery_margin`: 交易所交割保证金<br>`reserve_balance`: 保底期货结算准备金<br>`currency_id`: 币种代码<br>以及其他账户字段 |
| 13 | `ORDER_INSERT_ERROR` | 报单插入错误 | `req_id`: 请求ID<br>`is_last`: 是否最后一条 |
| 14 | `ORDER_INSERTED` | 报单插入成功 | `broker_id`: 经纪商代码<br>`investor_id`: 投资者代码<br>`user_id`: 用户代码<br>`exchange_id`: 交易所代码<br>`req_id`: 请求ID<br>`ref`: 报单引用<br>`order_local_id`: 本地报单编号<br>`order_sys_id`: 系统报单编号<br>`instrument_id`: 合约代码<br>`insert_date`: 报单日期<br>`insert_time`: 报单时间<br>`order_submit_status`: 报单提交状态<br>`order_status`: 报单状态<br>`volume_traded`: 今成交数量<br>`volume_total`: 剩余数量<br>`status_msg`: 状态信息 |
| 15 | `ORDER_TRADED` | 报单成交 | `broker_id`: 经纪商代码<br>`investor_id`: 投资者代码<br>`user_id`: 用户代码<br>`ref`: 报单引用<br>`exchange_id`: 交易所代码<br>`trade_id`: 成交编号<br>`order_sys_id`: 系统报单编号<br>`order_local_id`: 本地报单编号<br>`broker_order_seq`: 经纪公司报单编号<br>`settlement_id`: 结算编号<br>`volume`: 成交数量 |
| 16 | `QUERY_ORDER` | 查询报单响应 | `broker_id`: 经纪商代码<br>`investor_id`: 投资者代码<br>`user_id`: 用户代码<br>`exchange_id`: 交易所代码<br>`req_id`: 请求ID<br>`ref`: 报单引用<br>`order_local_id`: 本地报单编号<br>`order_sys_id`: 系统报单编号<br>`instrument_id`: 合约代码<br>`insert_date`: 报单日期<br>`insert_time`: 报单时间<br>`order_submit_status`: 报单提交状态<br>`order_status`: 报单状态<br>`volume_traded`: 今成交数量<br>`volume_total`: 剩余数量<br>`status_msg`: 状态信息<br>`is_last`: 是否最后一条 |

