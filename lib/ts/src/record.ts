import * as CTP from "./CTP";
import * as fs from "fs";
import config from "./test.config.local.json";

const md = new CTP.MarketData(config.client.broker_id, config.client.user_id);
const trade = new CTP.Trade(config.client.broker_id, config.client.user_id);

md.setFront(config.front.addr_md, config.front.port_md);
trade.setFront(config.front.addr_trade, config.front.port_trade);

md.onInit = (d) => {
    console.log("Connected to WebCTP server (MarketData), info:", d);
}

md.onConnected = (d) => {
    console.log("Connected to front.");
    md.login(config.client.password);
}

md.onLogin = (d) => {
    console.log("MarketData Login.");
}

md.onMarketData = async (d) => {
    const filename = `/var/lib/webctp/record/${d.TradingDay}/${d.InstrumentID}.csv`;
    try {
        if (!fs.existsSync(filename))
            fs.writeFileSync(filename, "TradingDay,InstrumentID,UpdateTime,UpdateMillisec,LastPrice,Volume,BidPrice1,BidVolume1,AskPrice1,AskVolume1,AveragePrice,Turnover,OpenInterest,UpperLimitPrice,LowerLimitPrice\n");
        fs.appendFile(filename, `${d.TradingDay},${d.InstrumentID},${d.UpdateTime},${d.UpdateMillisec},${d.LastPrice.toFixed(2)},${d.Volume},${d.BidPrice1.toFixed(2)},${d.BidVolume1},${d.AskPrice1.toFixed(2)},${d.AskVolume1},${d.AveragePrice.toFixed(2)},${d.Turnover},${d.OpenInterest},${d.UpperLimitPrice},${d.LowerLimitPrice}\n`, ()=>{});
    }
    catch(e) {
        console.log(`Write file ${filename} failed, exception:`, e);
    }
}

trade.onInit = (d) => {
    console.log("Connected to WebCTP server (Trading), info:", d);
}

trade.onError = (data) => {
    console.error("Trade API Error:", data);
};

trade.onConnected = (data) => {
    console.log("Connected to CTP.");
    trade.set(config.client.broker_id, config.client.user_id);
    trade.auth(config.client.user_id, config.client.app_id, config.client.auth_code);
};

trade.onTradingDay = (data) => {
    console.log("Trading day: " + data.info.trading_day);
};

trade.onDisconnected = (data) => {
    console.log("Disconnected. Message:", data);
};

trade.onAuthenticate = (data) => {
    console.log("Authenticated. Message:", data);
    trade.login(config.client.user_id, config.client.password);
};

trade.onLogin = (data) => {
    console.log("Trade Logged in. Message:", data);
    if (data.info && data.info.trading_day) {
        const trading_day = data.info.trading_day;
        console.log("Trading day: " + trading_day);
        if (!fs.existsSync('/var/lib/webctp'))
            fs.mkdirSync('/var/lib/webctp');
        if (!fs.existsSync('/var/lib/webctp/record'))
            fs.mkdirSync('/var/lib/webctp/record');
        if (!fs.existsSync(`/var/lib/webctp/record/${trading_day}`))
            fs.mkdirSync(`/var/lib/webctp/record/${trading_day}`);
    }
    const utc8Time = new Date((new Date()).getTime() + (8 * 60 * 60 * 1000) - (24 * 60 * 60 * 1000));
    const year = utc8Time.getUTCFullYear();
    const month = String(utc8Time.getUTCMonth() + 1).padStart(2, '0');
    const day = String(utc8Time.getUTCDate()).padStart(2, '0');
    const dateStr = `${year}${month}${day}`;
    trade.querySettlementInfo(dateStr);
};

trade.onLogout = (data) => {
    console.log("Logged out. Message:", data);
};

trade.onSettlementInfo = (data) => {
    if (data.Content)
        console.log("Settlement info: \n" + JSON.stringify(data.Content).replace(/\\r\\n/g, "\n"));
    if (data.IsLast) {
        trade.confirmSettlementInfo();
    }
};

trade.onSettlementInfoConfirm = (data) => {
    console.log("Settlement info confirmed:", data);
    trade.queryInstrument();
    // md.subscribe(['jm2601'])
};

const instruments: string[] = [];

trade.onQueryInstrument = (data) => {
    if (data.UnderlyingInstrID.length <= 2)
        instruments.push(data.InstrumentID);
    if (data.IsLast) {
        instruments.forEach((v) => {
            md.subscribe([v]);
        });
    }
}

trade.connect(config.webctp.addr, config.webctp.port);
md.connect(config.webctp.addr, config.webctp.port);

setInterval(() => {
    const now = new Date();
    const hours = now.getHours();
    const minutes = now.getMinutes();

    // 8:40 Login
    if (hours === 8 && minutes === 40) {
        console.log("Attempt to connect...");
        trade.connect(config.webctp.addr, config.webctp.port);
        md.connect(config.webctp.addr, config.webctp.port);
        return;
    }
    // 11:40 Unsubscribe, Logout, Disconnect
    if (hours === 11 && minutes === 40) {
        console.log("Attempt to disconnect...");
        if (instruments.length > 0) {
            md.unsubscribe(instruments);
            instruments.length = 0;
        }
        trade.logout(config.client.user_id);
        trade.disconnect();
        md.disconnect();
        return;
    }
    // 13:20 Login
    if (hours === 13 && minutes === 20) {
        console.log("Attempt to connect...");
        trade.connect(config.webctp.addr, config.webctp.port);
        md.connect(config.webctp.addr, config.webctp.port);
        return;
    }
    // 15:30 Unsubscribe, Logout, Disconnect
    if (hours === 15 && minutes === 30) {
        console.log("Attempt to disconnect...");
        if (instruments.length > 0) {
            md.unsubscribe(instruments);
            instruments.length = 0;
        }
        trade.logout(config.client.user_id);
        trade.disconnect();
        md.disconnect();
        return;
    }
    // 20:50 Login
    if (hours === 20 && minutes === 50) {
        console.log("Attempt to connect...");
        trade.connect(config.webctp.addr, config.webctp.port);
        md.connect(config.webctp.addr, config.webctp.port);
        return;
    }
    // 2:40 Unsubscribe, Logout, Disconnect
    if (hours === 2 && minutes === 40) {
        console.log("Attempt to disconnect...");
        if (instruments.length > 0) {
            md.unsubscribe(instruments);
            instruments.length = 0;
        }
        trade.logout(config.client.user_id);
        trade.disconnect();
        md.disconnect();
        return;
    }
}, 1000 * 60);