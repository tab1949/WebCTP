import * as CTP from "../src/CTP";
import * as fs from "fs";
import config from "./test.config.local.json";
import { spawn, spawnSync } from "child_process";
import log4js from "log4js";

process.on('uncaughtException', (err) => {
    logger.error('Uncaught Exception:', err);
    process.exit(1);
});
process.on('unhandledRejection', (reason) => {
    logger.error('Unhandled Rejection:', reason);
});

const md = new CTP.MarketData(config.client.broker_id, config.client.user_id);
const trade = new CTP.Trade(config.client.broker_id, config.client.user_id);
let trading_day: string = '';

// Setup logger
try { 
    if (!fs.existsSync('/var/lib/webctp/logs')) {
        fs.mkdirSync('/var/lib/webctp/logs', { recursive: true });
    }
} catch (e) {};

log4js.configure({
    appenders: {
        out: { 
            type: 'stdout', 
            layout: { 
                type: 'pattern', 
                pattern: '%[[%d{yyyy-MM-dd hh:mm:ss}] [%p]%] %m'
            } 
        },
        file: {
            type: 'dateFile',
            filename: '/var/lib/webctp/logs/record.log',
            pattern: 'yyyy-MM-dd',
            compress: true,
            keepFileExt: true,
            layout: { type: 'pattern', pattern: '[%d{yyyy-MM-dd hh:mm:ss}] [%p] %m' }
        }
    },
    categories: { default: { appenders: ['out', 'file'], level: process.env.LOG_LEVEL || 'info' } }
});
const logger = log4js.getLogger('record');

const safeFunc = <T extends any[]>(fn: (...args: T) => any) => {
    return (...args: T) => {
        try { return fn(...args); } catch (e) { logger.error('Handler error:', e); }
    };
};

const ensureDir = (dir: string) => {
    try { 
        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }
    } catch (e) { 
        logger.error('EnsureDir error:', dir, e); 
    }
};

md.onError = safeFunc((data) => {
    logger.error("MarketData API Error:", data);
});

md.onPerformed = safeFunc((d) => {
    logger.info("MarketData API Performed:", d);

});

md.onInit = safeFunc((d) => {
    logger.info("Connected to WebCTP server (MarketData), info:", d);
    md.connectFront(config.front.addr_md, config.front.port_md);
});

md.onConnected = safeFunc((d) => {
    logger.info("Connected to front.");
    try { 
        md.login(config.client.password); 
    } catch (e) {
        logger.error('md.login error:', e); 
    }
});

md.onLogin = safeFunc((d) => {
    logger.info("MarketData Login.");
});

md.onMarketData = safeFunc((d) => {
    const filename = `/var/lib/webctp/record/${d.TradingDay}/${d.InstrumentID}.csv`;
    try {
        if (!fs.existsSync(filename))
            fs.writeFileSync(filename, "TradingDay,InstrumentID,UpdateTime,UpdateMillisec,LastPrice,Volume,BidPrice1,BidVolume1,AskPrice1,AskVolume1,AveragePrice,Turnover,OpenInterest,UpperLimitPrice,LowerLimitPrice\n");
        fs.appendFile(filename, `${d.TradingDay},${d.InstrumentID},${d.UpdateTime},${d.UpdateMillisec},${d.LastPrice.toFixed(2)},${d.Volume},${d.BidPrice1.toFixed(2)},${d.BidVolume1},${d.AskPrice1.toFixed(2)},${d.AskVolume1},${d.AveragePrice.toFixed(2)},${d.Turnover},${d.OpenInterest},${d.UpperLimitPrice},${d.LowerLimitPrice}\n`, ()=>{});
    }
    catch(e) {
        logger.error(`Write file ${filename} failed, exception:`, e);
    }
});

trade.onPerformed = safeFunc((d) => {
    logger.info("Trade API Performed:", d);
});

trade.onInit = safeFunc((d) => {
    logger.info("Connected to WebCTP server (Trading), info:", d);
    trade.connectFront(config.front.addr_trade, config.front.port_trade);
});

trade.onError = safeFunc((data) => {
    logger.error("Trade API Error:", data);
});

trade.onConnected = safeFunc((data) => {
    logger.info("Connected to CTP.");
    try {
        trade.set(config.client.broker_id, config.client.user_id);
        trade.auth(config.client.user_id, config.client.app_id, config.client.auth_code);
    } catch (e) { 
        logger.error('Trade.set/auth error:', e); 
    }
});

trade.onTradingDay = safeFunc((data) => {
    logger.info("Trading day: " + data.info.trading_day);
});

trade.onDisconnected = safeFunc((data) => {
    logger.warn("Disconnected. Message:", data);
});

trade.onAuthenticate = safeFunc((data) => {
    logger.info("Authenticated. Message:", data);
    try { 
        trade.login(config.client.user_id, config.client.password); 
    } catch (e) { 
        logger.error('trade.login error:', e); 
    }
});

trade.onLogin = safeFunc((data) => {
    logger.info("Trade Logged in. Message:", data);
    if (data.info && data.info.trading_day) {
        trading_day = data.info.trading_day;
        logger.info("Trading day: " + trading_day);
        try {
            ensureDir('/var/lib/webctp');
            ensureDir('/var/lib/webctp/record');
            ensureDir('/var/lib/webctp/record/archived');
            ensureDir(`/var/lib/webctp/record/${trading_day}`);
        } catch (e) { 
            logger.error('mkdir error:', e); 
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
        logger.error('querySettlementInfo error:', e); 
    }
});

trade.onLogout = safeFunc((data) => {
    logger.info("Logged out. Message:", data);
});

let settlementCount: number = 0;
trade.onSettlementInfo = safeFunc((data) => {
    try {
        if (data?.IsLast) {
            logger.info(`${settlementCount + 1} lines of settlement info received.`);
            settlementCount = 0;
            trade.confirmSettlementInfo();
        }
        else {
            settlementCount ++;
        }
    } catch (e) { 
        logger.error('onSettlementInfo error:', e); 
    }
});

trade.onSettlementInfoConfirm = safeFunc((data) => {
    if (data.IsLast)
        logger.info(`Settlement info confirmed. Confirm date: ${data.ConfirmDate}, time: ${data.ConfirmTime}`);
    try { 
        trade.queryInstrument(); 
    } catch (e) { 
        logger.error('queryInstrument error:', e); 
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
                    logger.error('md.subscribe error:', v, e); 
                }
            });
        }
    } catch (e) { 
        logger.error('onQueryInstrument error:', e); 
    }
});

const stop = () => {
    try { 
        trade.logout(config.client.user_id); 
    } catch (e) { logger.error('trade.logout error:', e); }

    try { 
        trade.disconnect(); 
    } catch (e) { logger.error('trade.disconnect error:', e); }
    try { 
        md.disconnect(); 
    } catch (e) { logger.error('md.disconnect error:', e); }

    log4js.shutdown((err) => {
        if (err) {
            console.error("Failed to shutdown log4js:", err);
        }
        process.exit(0);
    });
};

const connect = () => {
    try { 
        trade.connect(config.webctp.addr, config.webctp.port); 
    } catch (e) { logger.error('trade.connect error:', e); }

    try { 
        md.connect(config.webctp.addr, config.webctp.port); 
    } catch (e) { logger.error('md.connect error:', e); }
};

process.on('SIGINT', stop);
process.on('SIGTERM', stop);
process.on('exit', () => log4js.shutdown());

interface Time {
    hour: number;
    minute: number;
};

const getUTC8 = (): Time => {
    try {
        const now = new Date();
        return {
            hour: (now.getUTCHours() + 8) % 24,
            minute: now.getUTCMinutes()
        };
    }
    catch (e) {
        throw `Exception caught in getUTC8(): ${e}`;
    }
}

const validateTime = (): boolean => {
    const now = getUTC8();
    const h = now.hour;
    const m = now.minute;
    const minutes = h * 60 + m;
    const toMinutes = (hh: number, mm: number) => hh * 60 + mm;
    if (minutes >= toMinutes(8, 40) && minutes < toMinutes(11, 40)) return true;
    if (minutes >= toMinutes(13, 20) && minutes < toMinutes(15, 10)) return true;
    if (minutes >= toMinutes(20, 50) || minutes < toMinutes(2, 40)) return true;
    return false;
};

try {
    if (validateTime()) {
        logger.info('Valid time. Connect immediately...');
        connect();
    } else {
        logger.info('Invalid time. Waiting for trigger...');
    }
} catch (e) {
    logger.error('Initial trading time check error:', e);
}

setInterval(() => {
    try {
        const now = getUTC8();
        const hours = now.hour;
        const minutes = now.minute;

        if (hours === 8 && minutes === 40) { logger.info("Attempt to connect..."); connect(); return; }
        if (hours === 11 && minutes === 40) { logger.info("Attempt to pause..."); stop(); return; }
        if (hours === 13 && minutes === 20) { logger.info("Attempt to connect..."); connect(); return; }
        if (hours === 15 && minutes === 10) { logger.info("Attempt to pause..."); stop();
            if (!trading_day) {
                logger.warn("trading_day is not set, skip archiving.");
                return;
            }
            try {
                const tarRes = spawnSync('tar', ['-cf', `/var/lib/webctp/record/archived/${trading_day}.tar`, `/var/lib/webctp/record/${trading_day}`]);
                if (tarRes.error) logger.error('tar error:', tarRes.error);
            } catch (e) { logger.error('tar spawnSync error:', e); }
            try {
                const rmProc = spawn('rm', ['-r', `/var/lib/webctp/record/${trading_day}`]);
                rmProc.on('error', (e) => logger.error('rm error:', e));
                rmProc.on('close', (code, signal) => logger.info('rm close:', code, signal));
            } catch (e) { logger.error('rm spawn error:', e); }
            try {
                const gzProc = spawn('gzip', ['-9', `/var/lib/webctp/record/archived/${trading_day}.tar`]);
                gzProc.on('error', (e) => logger.error('gzip error:', e));
                gzProc.on('close', (code, signal) => logger.info('gzip close:', code, signal));
            } catch (e) { logger.error('gzip spawn error:', e); }
            return;
        }
        if (hours === 20 && minutes === 50) { logger.info("Attempt to connect..."); connect(); return; }
        if (hours === 2 && minutes === 40) { logger.info("Attempt to pause..."); stop(); return; }
    } catch (e) {
        logger.error('setInterval error:', e);
    }
}, 1000 * 60);