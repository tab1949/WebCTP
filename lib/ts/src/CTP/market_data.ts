import * as ws from "ws";
import * as Message from "./message";

export class MarketData {
    public onInit: (data: any) => void = () => {};
    public onConnected: (data: any) => void = () => {};
    public onDisconnected: (data: any) => void = () => {}
    public onPerformed: (data: any) => void = () => {};
    public onError: (data: any) => void = () => {};
    public onFrontConnected: (data: any) => void = () => {};
    public onFrontDisconnected: (data: any) => void = () => {};
    public onHeartbeatTimeout: (data: any) => void = () => {};
    public onLogin: (data: any) => void = () => {};
    public onLogout: (data: any) => void = () => {};
    public onTradingDay: (data: any) => void = () => {};
    public onSubscribe: (data: any) => void = () => {};
    public onUnsubscribe: (data: any) => void = () => {};
    public onMarketData: (data: Message.MarketData) => void = () => {};

    private ws: ws.WebSocket | undefined;
    private tradingDay: string | undefined;
    private brokerID: string;
    private userID: string;

    constructor(brokerID: string = "", userID: string = "") {
        this.brokerID = brokerID;
        this.userID = userID;
    }

    public connectFront(addr: string, port: string) {
        this.ws?.send(JSON.stringify({
            op: "connect",
            data: {
                addr: addr,
                port: port
            }
        }));
    }

    public connect(addr: string, port: string) {
        if (this.ws) {
            return;
        }
        this.ws = new ws.WebSocket(`ws://${addr}:${port}/market_data`);
        this.ws.onclose = () => {
            this.ws = undefined;
            this.onDisconnected("WebCTP.MarketData: WebSocket closed");
        };
        this.ws.onerror = (event) => {
            this.onError("WebCTP.MarketData: WebSocket Error: " + JSON.stringify(event));
        };
        this.ws.onopen = () => {
            this.onConnected("WebCTP.MarketData: WebSocket opened");
        };
        this.ws.onmessage = (event) => {
            let data: any;
            try {
                data = JSON.parse(event.data.toString());
            } catch (error) {
                this.onError("Parse message failed: " + error);
                return;
            }
            if (!this.ws) {
                this.onError("onmessage(): WebSocket is not available");
                return;
            }
            if (typeof data.msg === "string") {
                switch (data.msg) {
                case "ready":
                    this.onInit(data);
                    return;
                case "parse_error":
                case "processing_error":
                case "error":
                    this.onError(data);
                    return;
                }
            }
            switch (data.msg) {
            case Message.MDMsgCode.PERFORMED:
                this.onPerformed(data);
                break;
            case Message.MDMsgCode.ERROR:
                this.onError(data);
                break;
            case Message.MDMsgCode.CONNECTED:
                this.onFrontConnected(data);
                break;
            case Message.MDMsgCode.DISCONNECTED:
                this.onFrontDisconnected(data);
                break;
            case Message.MDMsgCode.HEARTBEAT_TIMEOUT:
                this.onHeartbeatTimeout(data);
                break;
            case Message.MDMsgCode.LOGIN:
                if (data.info && data.info.trading_day) {
                    this.tradingDay = data.info.trading_day;
                }
                this.onLogin(data);
                break;
            case Message.MDMsgCode.LOGOUT:
                this.onLogout(data);
                break;
            case Message.MDMsgCode.TRADING_DAY:
                this.tradingDay = data.info.trading_day;
                this.onTradingDay(data);
                break;
            case Message.MDMsgCode.SUBSCRIBE:
                this.onSubscribe(data);
                break;
            case Message.MDMsgCode.UNSUBSCRIBE:
                this.onUnsubscribe(data);
                break;
            case Message.MDMsgCode.MARKET_DATA:
                if (data.info) {
                    const d: Message.MarketData = {
                        TradingDay: data.info.trading_day,
                        InstrumentID: data.info.instrument_id,
                        ExchangeID: data.info.exchange_id,
                        ExchangeInstID: data.info.exchange_inst_id,
                        LastPrice: data.info.last_price,
                        PreSettlementPrice: data.info.pre_settlement_price,
                        PreClosePrice: data.info.pre_close_price,
                        PreOpenInterest: data.info.pre_open_interest,
                        OpenPrice: data.info.open_price,
                        HighestPrice: data.info.highest_price,
                        LowestPrice: data.info.lowest_price,
                        Volume: data.info.volume,
                        Turnover: data.info.turnover,
                        OpenInterest: data.info.open_interest,
                        ClosePrice: data.info.close_price,
                        SettlementPrice: data.info.settlement_price,
                        UpperLimitPrice: data.info.upper_limit_price,
                        LowerLimitPrice: data.info.lower_limit_price,
                        PreDelta: data.info.pre_delta,
                        CurrDelta: data.info.curr_delta,
                        UpdateTime: data.info.update_time,
                        UpdateMillisec: data.info.update_millisec,
                        BidPrice1: data.info.bp1,
                        BidVolume1: data.info.bv1,
                        AskPrice1: data.info.ap1,
                        AskVolume1: data.info.av1,
                        BidPrice2: data.info.bp2,
                        BidVolume2: data.info.bv2,
                        AskPrice2: data.info.ap2,
                        AskVolume2: data.info.av2,
                        BidPrice3: data.info.bp3,
                        BidVolume3: data.info.bv3,
                        AskPrice3: data.info.ap3,
                        AskVolume3: data.info.av3,
                        BidPrice4: data.info.bp4,
                        BidVolume4: data.info.bv4,
                        AskPrice4: data.info.ap4,
                        AskVolume4: data.info.av4,
                        BidPrice5: data.info.bp5,
                        BidVolume5: data.info.bv5,
                        AskPrice5: data.info.ap5,
                        AskVolume5: data.info.av5,
                        AveragePrice: data.info.average_price,
                        ActionDay: data.info.action_day,
                        BandingUpperPrice: data.info.banding_upper_price,
                        BandingLowerPrice: data.info.banding_lower_price,
                    };
                    this.onMarketData(d);
                }
                break;
            default:
                this.onError("Unknown message: " + JSON.stringify(data));
                return;
            }
        };
    }

    public login(password: string) {
        if (!this.ws) {
            this.onError("WebCTP.MarketData.login(): WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "login",
            data: {
                broker_id: this.brokerID,
                user_id: this.userID,
                password: password
            }
        }));
    }

    public subscribe(instruments: string[]) {
        if (!this.ws) {
            this.onError("WebCTP.MarketData.subscribe(): WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "subscribe",
            data: {
                instruments: instruments
            }
        }));
    }

    public unsubscribe(instruments: string[]) {
        if (!this.ws) {
            this.onError("WebCTP.MarketData.unsubscribe(): WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "unsubscribe",
            data: {
                instruments: instruments
            }
        }));
    }

    public getTradingDay() {
        if (!this.ws) {
            this.onError("WebCTP.MarketData.getTradingDay(): WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "get_trading_day",
            data: {}
        }));
    }

    public setBrokerID(brokerID: string) {
        this.brokerID = brokerID;
    }

    public setUserID(userID: string) {
        this.userID = userID;
    }

    public disconnect() {
        if (!this.ws)
            return;
        this.ws.close();
        this.ws = undefined;
    }
}