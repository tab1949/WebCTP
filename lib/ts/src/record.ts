import * as CTP from "./CTP";
import * as fs from "fs";
import config from "./test.config.local.json";
import { spawn, spawnSync } from "child_process";

const md = new CTP.MarketData(config.client.broker_id, config.client.user_id);
const trade = new CTP.Trade(config.client.broker_id, config.client.user_id);
let trading_day: string = '';

// Global safety nets: log and keep process alive
process.on('uncaughtException', (err) => {
    console.error('[uncaughtException]', err);
});
process.on('unhandledRejection', (reason, promise) => {
    console.error('[unhandledRejection]', reason);
});

const safeFunc = <T extends any[]>(fn: (...args: T) => any) => {
    return (...args: T) => {
        try { return fn(...args); } catch (e) { console.error('[handler error]', e); }
    };
};

const ensureDir = (dir: string) => {
    try { fs.mkdirSync(dir, { recursive: true }); } catch (e) { console.error('[ensureDir]', dir, e); }
};


md.onInit = safeFunc((d) => {
    console.log("Connected to WebCTP server (MarketData), info:", d);
});

md.onConnected = safeFunc((d) => {
    console.log("Connected to front.");
    try { 
        md.login(config.client.password); 
    } catch (e) {
        console.error('[md.login]', e); 
    }
});

md.onLogin = safeFunc((d) => {
    console.log("MarketData Login.");
});

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

trade.onInit = safeFunc((d) => {
    console.log("Connected to WebCTP server (Trading), info:", d);
});

trade.onError = safeFunc((data) => {
    console.error("Trade API Error:", data);
});

trade.onConnected = safeFunc((data) => {
    console.log("Connected to CTP.");
    try {
        trade.set(config.client.broker_id, config.client.user_id);
        trade.auth(config.client.user_id, config.client.app_id, config.client.auth_code);
    } catch (e) { 
        console.error('[trade.set/auth]', e); 
    }
});

trade.onTradingDay = safeFunc((data) => {
    console.log("Trading day: " + data.info.trading_day);
});

trade.onDisconnected = safeFunc((data) => {
    console.log("Disconnected. Message:", data);
});

trade.onAuthenticate = safeFunc((data) => {
    console.log("Authenticated. Message:", data);
    try { 
        trade.login(config.client.user_id, config.client.password); 
    } catch (e) { 
        console.error('[trade.login]', e); 
    }
});

trade.onLogin = safeFunc((data) => {
    console.log("Trade Logged in. Message:", data);
    if (data.info && data.info.trading_day) {
        trading_day = data.info.trading_day;
        console.log("Trading day: " + trading_day);
        try {
            ensureDir('/var/lib/webctp');
            ensureDir('/var/lib/webctp/record');
            ensureDir('/var/lib/webctp/record/archived');
            ensureDir(`/var/lib/webctp/record/${trading_day}`);
        } catch (e) { 
            console.error('[mkdir]', e); 
        }
    }
    const utc8Time = new Date((new Date()).getTime() + (8 * 60 * 60 * 1000) - (24 * 60 * 60 * 1000));
    const year = utc8Time.getUTCFullYear();
    const month = String(utc8Time.getUTCMonth() + 1).padStart(2, '0');
    const day = String(utc8Time.getUTCDate()).padStart(2, '0');
    const dateStr = `${year}${month}${day}`;
    try { 
        trade.querySettlementInfo(dateStr); 
    } catch (e) { 
        console.error('[querySettlementInfo]', e); 
    }
});

trade.onLogout = safeFunc((data) => {
    console.log("Logged out. Message:", data);
});

trade.onSettlementInfo = safeFunc((data) => {
    try {
        if (data?.Content)
            console.log("Settlement info: \n" + JSON.stringify(data.Content).replace(/\\r\\n/g, "\n"));
        if (data?.IsLast) {
            trade.confirmSettlementInfo();
        }
    } catch (e) { 
        console.error('[onSettlementInfo]', e); 
    }
});

trade.onSettlementInfoConfirm = safeFunc((data) => {
    console.log("Settlement info confirmed:", data);
    try { 
        trade.queryInstrument(); 
    } catch (e) { 
        console.error('[queryInstrument]', e); 
    }
});

const instruments: string[] = [];

trade.onQueryInstrument = safeFunc((data) => {
    try {
        if (data.UnderlyingInstrID.length <= 2 && data.InstrumentID)
            instruments.push(data.InstrumentID);
        if (data.IsLast) {
            instruments.forEach((v) => {
                try { 
                    md.subscribe([v]); 
                } catch (e) { 
                    console.error('[md.subscribe]', v, e); 
                }
            });
        }
    } catch (e) { 
        console.error('[onQueryInstrument]', e); 
    }
});

const stop = () => {
    if (instruments.length > 0) {
        try { 
            md.unsubscribe(instruments); 
        } catch (e) { console.error('[md.unsubscribe]', e); }
        instruments.length = 0;
    }

    try { 
        trade.logout(config.client.user_id); 
    } catch (e) { console.error('[trade.logout]', e); }

    try { 
        trade.disconnect(); 
    } catch (e) { console.error('[trade.disconnect]', e); }

    try { 
        md.disconnect(); 
    } catch (e) { console.error('[md.disconnect]', e); }
};

const connect = () => {
    try { 
        trade.connect(config.webctp.addr, config.webctp.port); 
        trade.connectFront(config.front.addr_trade, config.front.port_trade);
    } catch (e) { console.error('[trade.connect]', e); }

    try { 
        md.connect(config.webctp.addr, config.webctp.port); 
        md.connectFront(config.front.addr_md, config.front.port_md);
    } catch (e) { console.error('[md.connect]', e); }
    
};

// Check if current time is within trading hours
const isTradingTime = (): boolean => {
    const now = new Date();
    const hours = now.getHours();
    const minutes = now.getMinutes();
    const totalMinutes = hours * 60 + minutes;

    // Morning session: 8:40 - 11:40
    const morningStart = 8 * 60 + 40;  // 8:40
    const morningEnd = 11 * 60 + 40;   // 11:40

    // Afternoon session: 13:20 - 15:10
    const afternoonStart = 13 * 60 + 20;  // 13:20
    const afternoonEnd = 15 * 60 + 10;    // 15:10

    // Night session: 20:50 - 2:40 (next day)
    const nightStart = 20 * 60 + 50;  // 20:50
    const nightEnd = 2 * 60 + 40;     // 2:40

    // Check morning session (8:40 - 11:40)
    if (totalMinutes >= morningStart && totalMinutes < morningEnd) {
        return true;
    }

    // Check afternoon session (13:20 - 15:10)
    if (totalMinutes >= afternoonStart && totalMinutes < afternoonEnd) {
        return true;
    }

    // Check night session (20:50 - 2:40)
    // Night session spans midnight, so we need to check two ranges:
    // 20:50 - 23:59 OR 0:00 - 2:40
    if (totalMinutes >= nightStart || totalMinutes < nightEnd) {
        return true;
    }

    return false;
};

// If it is trading time now, connect immediately
if (isTradingTime()) {
    console.log("Trading time detected at startup, connecting immediately...");
    connect();
}

setInterval(() => {
    try {
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();

        // 8:40 Login
        if (hours === 8 && minutes === 40) {
            console.log("Attempt to connect...");
            connect();
            return;
        }
        // 11:40 Unsubscribe, Logout, Disconnect
        if (hours === 11 && minutes === 40) {
            console.log("Attempt to pause...");
            stop();
            return;
        }
        // 13:20 Login
        if (hours === 13 && minutes === 20) {
            console.log("Attempt to connect...");
            connect();
            return;
        }
        // 15:10 Unsubscribe, Logout, Disconnect
        if (hours === 15 && minutes === 10) {
            console.log("Attempt to pause...");
            stop();

            try {
                const tarRes = spawnSync('tar', ['-cf', `/var/lib/webctp/record/archived/${trading_day}.tar`, `/var/lib/webctp/record/${trading_day}`]);
                if (tarRes.error) 
                    console.error('[tar error]', tarRes.error);
            } catch (e) { console.error('[tar spawnSync]', e); }

            try {
                const rmProc = spawn('rm', ['-r', `/var/lib/webctp/record/${trading_day}`]);
                rmProc.on('error', (e) => console.error('[rm error]', e));
                rmProc.on('close', (code, signal) => console.log('[rm close]', code, signal));
            } catch (e) { console.error('[rm spawn]', e); }

            try {
                const gzProc = spawn('gzip', ['-9', `/var/lib/webctp/record/archived/${trading_day}.tar`]);
                gzProc.on('error', (e) => console.error('[gzip error]', e));
                gzProc.on('close', (code, signal) => console.log('[gzip close]', code, signal));
            } catch (e) { console.error('[gzip spawn]', e); }

            return;
        }
        // 20:50 Login
        if (hours === 20 && minutes === 50) {
            console.log("Attempt to connect...");
            connect();
            return;
        }
        // 2:40 Unsubscribe, Logout, Disconnect
        if (hours === 2 && minutes === 40) {
            console.log("Attempt to pause...");
            stop();
            return;
        }
    } catch (e) {
        console.error('[setInterval error]', e);
    }
}, 1000 * 60);

process.on('SIGINT', stop);
process.on('SIGTERM', stop);