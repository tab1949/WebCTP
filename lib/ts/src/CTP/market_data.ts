import * as ws from "ws";
import * as Message from "./message";

export class MarketData {
    public onInit: (data: any) => void = () => {};
    public onPerformed: (data: any) => void = () => {};
    public onError: (data: any) => void = () => {};
    public onConnected: (data: any) => void = () => {};
    public onDisconnected: (data: any) => void = () => {};
    public onHeartbeatTimeout: (data: any) => void = () => {};
    public onLogin: (data: any) => void = () => {};
    public onLogout: (data: any) => void = () => {};
    public onTradingDay: (data: any) => void = () => {};
    public onSubscribe: (data: any) => void = () => {};
    public onUnsubscribe: (data: any) => void = () => {};
    public onMarketData: (data: any) => void = () => {};

    private ws: ws.WebSocket | undefined;
    private tradingDay: string | undefined;
    private brokerID: string;
    private userID: string;
    private frontAddr: string | undefined;
    private frontPort: string | undefined;

    constructor(brokerID: string = "", userID: string = "") {
        this.brokerID = brokerID;
        this.userID = userID;
    }

    public connectFront(addr: string, port: string) {
        this.ws?.send(JSON.stringify({
            op: "connect",
            data: {
                front: {
                    address: addr,
                    port: port
                }
            }
        }));
    }

    public setFront(addr: string, port: string) {
        this.frontAddr = addr;
        this.frontPort = port;
    }

    public connect(addr: string, port: string) {
        if (!this.frontAddr || !this.frontPort) {
            throw new Error("Front address or port not set");
        }
        if (this.ws) {
            return;
        }
        this.ws = new ws.WebSocket(`ws://${addr}:${port}/market_data`);
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
            }
            if (!this.ws) {
                throw new Error("WebSocket is not available");
            }
            switch (data.msg) {
            case Message.MDMsgCode.PERFORMED:
                this.onPerformed(data);
                break;
            case Message.MDMsgCode.ERROR:
                this.onError(data);
                break;
            case Message.MDMsgCode.CONNECTED:
                this.onConnected(data);
                break;
            case Message.MDMsgCode.DISCONNECTED:
                this.onDisconnected(data);
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
                this.onMarketData(data);
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

    public login(password: string) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
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

    // public logout() {
    //     if (!this.ws) {
    //         throw new Error("WebSocket is not connected");
    //         return;
    //     }
    //     this.ws.send(JSON.stringify({
    //         op: "logout",
    //         data: {
    //             broker_id: this.brokerID,
    //             user_id: this.userID
    //         }
    //     }));
    // }

    public subscribe(instruments: string[]) {
        if (!this.ws) {
            throw new Error("WebSocket is not connected");
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
            throw new Error("WebSocket is not connected");
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
            throw new Error("WebSocket is not connected");
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
}