import * as ws from "ws";
import * as Flags from "./flags";
import * as Message from "./message";

export class Trade {
    public onInit: (data: any) => void = () => {};
    public onPerformed: (data: any) => void = () => {};
    public onError: (data: any) => void = () => {};
    public onErrorNull: (data: any) => void = () => {};
    public onErrorUnknownValue: (data: any) => void = () => {};
    public onConnected: (data: any) => void = () => {};
    public onTradingDay: (data: any) => void = () => {};
    public onDisconnected: (data: any) => void = () => {};
    public onAuthenticate: (data: any) => void = () => {};
    public onLogin: (data: any) => void = () => {};
    public onLogout: (data: any) => void = () => {};
    public onSettlementInfo: (data: any) => void = () => {};
    public onSettlementInfoConfirm: (data: any) => void = () => {};
    public onTradingAccount: (data: any) => void = () => {};
    public onOrderInsertError: (data: any) => void = () => {};
    public onOrderInserted: (data: any) => void = () => {};
    public onOrderTraded: (data: any) => void = () => {};
    public onQueryOrder: (data: any) => void = () => {};
    private brokerID: string;
    private investorID: string;
    private frontAddr: string | undefined;
    private frontPort: string | undefined;
    private ws: ws.WebSocket | undefined;
    private tradingDay: string | undefined;

    constructor(brokerID: string = "", investorID: string = "") {
        this.brokerID = brokerID;
        this.investorID = investorID;
    }

    public set(brokerID?: string, investorID?: string) {
        if (brokerID !== undefined) {
            this.brokerID = brokerID;
        }
        if (investorID !== undefined) {
            this.investorID = investorID;
        }
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        const data: any = {};
        if (brokerID !== undefined) {
            data.broker_id = brokerID;
        }
        if (investorID !== undefined) {
            data.investor_id = investorID;
        }
        this.ws.send(JSON.stringify({
            op: "set",
            data: data
        }));
    }

    public getTradingDay() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "get_trading_day",
            data: {}
        }));
    }

    public auth(userID: string, appID: string, authCode: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "auth",
            data: {
                user_id: userID,
                app_id: appID,
                auth_code: authCode
            }
        }));
    }

    public login(userID: string, password: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "login",
            data: {
                user_id: userID,
                password: password
            }
        }));
    }

    public logout(userID: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "logout",
            data: {
                user_id: userID
            }
        }));
    }

    public querySettlementInfo(tradingDay: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "query_settlement_info",
            data: {
                trading_day: tradingDay
            }
        }));
    }

    public confirmSettlementInfo() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "confirm_settlement_info",
            data: {}
        }));
    }

    public queryTradingAccount() {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "query_trading_account",
            data: {}
        }));
    }

    public insertOrder(
        instrument: string,
        exchange: string,
        ref: string,
        price: number,
        direction: number,
        offset: number,
        volume: number,
        priceType: number,
        timeCondition: number
    ) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        this.ws.send(JSON.stringify({
            op: "insert_order",
            data: {
                instrument: instrument,
                exchange: exchange,
                ref: ref,
                price: price,
                direction: direction,
                offset: offset,
                volume: volume,
                price_type: priceType,
                time_condition: timeCondition
            }
        }));
    }

    public queryOrder(options?: {
        orderSysID?: string,
        exchangeID?: string,
        from?: string,
        to?: string
    }) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
            return;
        }
        const data: any = {};
        if (options) {
            if (options.orderSysID !== undefined) {
                data.order_sys_id = options.orderSysID;
            }
            if (options.exchangeID !== undefined) {
                data.exchange_id = options.exchangeID;
            }
            if (options.from !== undefined) {
                data.from = options.from;
            }
            if (options.to !== undefined) {
                data.to = options.to;
            }
        }
        this.ws.send(JSON.stringify({
            op: "query_order",
            data: data
        }));
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

    public setFront(addr: string, port: string) {
        this.frontAddr = addr;
        this.frontPort = port;
    }

    public connect(addr: string, port: string) {
        if (this.ws) 
            return;
        if (!this.frontAddr || !this.frontPort) {
            throw new Error("Front address or port not set");
        }
        this.ws = new ws.WebSocket(`ws://${addr}:${port}/trade`);
        this.ws.onclose = () => {
            this.ws = undefined;
        };
        this.ws.onerror = (event) => {
            console.error("Error: " + JSON.stringify(event));
        };
        this.ws.onopen = () => {
            
        };
        this.ws.onmessage = (event) => {
            let data: any;
            try {
                data = JSON.parse(event.data.toString());
            } catch (error) {
                throw new Error("Parse message failed: " + error);
                return;
            }
            if (!this.ws) {
                throw new Error("WebSocket is not available");
                return;
            }
            switch (data.msg) {
            case Message.TradeMsgCode.PERFORMED:
                this.onPerformed(data);
                break;
            case Message.TradeMsgCode.ERROR:
                this.onError(data);
                break;
            case Message.TradeMsgCode.ERROR_NULL:
                this.onErrorNull(data);
                break;
            case Message.TradeMsgCode.ERROR_UNKNOWN_VALUE:
                this.onErrorUnknownValue(data);
                break;
            case Message.TradeMsgCode.CONNECTED:
                this.onConnected(data);
                break;
            case Message.TradeMsgCode.TRADING_DAY:
                if (data.info && data.info.trading_day) {
                    this.tradingDay = data.info.trading_day;
                }
                this.onTradingDay(data);
                break;
            case Message.TradeMsgCode.DISCONNECTED:
                this.onDisconnected(data);
                break;
            case Message.TradeMsgCode.AUTHENTICATE:
                this.onAuthenticate(data);
                break;
            case Message.TradeMsgCode.LOGIN:
                if (data.info && data.info.trading_day) {
                    this.tradingDay = data.info.trading_day;
                }
                this.onLogin(data);
                break;
            case Message.TradeMsgCode.LOGOUT:
                this.onLogout(data);
                break;
            case Message.TradeMsgCode.SETTLEMENT_INFO:
                this.onSettlementInfo(data);
                break;
            case Message.TradeMsgCode.SETTLEMENT_INFO_CONFIRM:
                this.onSettlementInfoConfirm(data);
                break;
            case Message.TradeMsgCode.TRADING_ACCOUNT:
                this.onTradingAccount(data);
                break;
            case Message.TradeMsgCode.ORDER_INSERT_ERROR:
                this.onOrderInsertError(data);
                break;
            case Message.TradeMsgCode.ORDER_INSERTED:
                this.onOrderInserted(data);
                break;
            case Message.TradeMsgCode.ORDER_TRADED:
                this.onOrderTraded(data);
                break;
            case Message.TradeMsgCode.QUERY_ORDER:
                this.onQueryOrder(data);
                break;
            default:
                if (data.status && data.status == "ready") {
                    this.onInit(data);
                    if (this.ws) {
                        this.ws.send(JSON.stringify({
                            op: "connect",
                            data: {
                                addr: this.frontAddr,
                                port: this.frontPort
                            }
                        }));
                    }
                    break;
                }
                throw new Error("Unknown message: " + JSON.stringify(data));
            }
        };
    }
}; // class Trade