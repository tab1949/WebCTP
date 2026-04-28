import * as CTP from "../../src/CTP";
import * as fs from "fs";
import config from "./test.config.local.json";
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

const MARKET_DATA_DIR = '/var/lib/webctp/record';
const MARKET_DATA_HEADER = 'TradingDay,InstrumentID,UpdateTime,UpdateMillisec,LastPrice,Volume,BidPrice1,BidVolume1,AskPrice1,AskVolume1,AveragePrice,Turnover,OpenInterest,UpperLimitPrice,LowerLimitPrice\n';
const marketDataBuffer: string[] = [];
let marketDataFlushInterval: NodeJS.Timeout;

const getMarketDataFilePath = () => {
    const dayFolder = trading_day || 'unknown';
    return `${MARKET_DATA_DIR}/${dayFolder}/market.csv`;
};

const flushMarketData = () => {
    if (marketDataBuffer.length === 0) {
        return;
    }

    const dataToWrite = marketDataBuffer.join('');
    marketDataBuffer.length = 0;

    const filePath = getMarketDataFilePath();
    ensureDir(`${MARKET_DATA_DIR}/${trading_day || 'unknown'}`);

    try {
        if (!fs.existsSync(filePath)) {
            fs.writeFileSync(filePath, MARKET_DATA_HEADER, 'utf8');
        }
        fs.appendFile(filePath, dataToWrite, 'utf8', (err) => {
            if (err) {
                logger.error('Async flush market data error:', err);
            }
        });
    } catch (e) {
        logger.error('Flush market data setup error:', e);
    }
};

const startMarketDataFlushLoop = () => {
    marketDataFlushInterval = setInterval(flushMarketData, 1000);
    if (marketDataFlushInterval?.unref) {
        marketDataFlushInterval.unref();
    }
};

const stopMarketDataFlushLoop = () => {
    if (marketDataFlushInterval) {
        clearInterval(marketDataFlushInterval);
        marketDataFlushInterval = undefined as unknown as NodeJS.Timeout;
    }
    flushMarketData();
};

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
    md.connectFront("", config.front.addr_md, config.front.port_md);
});

md.onConnected = safeFunc((d) => {
    logger.info("MarketData WebSocket connected:", d);
});

md.onDisconnected = safeFunc((d) => {
    logger.warn("MarketData WebSocket disconnected:", d);
});

md.onFrontConnected = safeFunc((d) => {
    logger.info("Connected to front.");
    try { 
        md.login("", config.client.password); 
    } catch (e) {
        logger.error('md.login error:', e); 
    }
});

md.onLogin = safeFunc((d) => {
    logger.info("MarketData Login.");
    try { 
        trade.queryInstrument(""); 
    } catch (e) { 
        logger.error('queryInstrument error:', e); 
    }
});

md.onMarketData = safeFunc((d) => {
    const line = `${d.TradingDay},${d.InstrumentID},${d.UpdateTime},${d.UpdateMillisec},${d.LastPrice.toFixed(3)},${d.Volume},${d.BidPrice1.toFixed(3)},${d.BidVolume1},${d.AskPrice1.toFixed(3)},${d.AskVolume1},${d.AveragePrice.toFixed(3)},${d.Turnover},${d.OpenInterest},${d.UpperLimitPrice.toFixed(3)},${d.LowerLimitPrice.toFixed(3)}\n`;
    marketDataBuffer.push(line);
});

trade.onPerformed = safeFunc((d) => {
    logger.info("Trade API Performed:", d);
});

trade.onInit = safeFunc((d) => {
    logger.info("Connected to WebCTP server (Trading), info:", d);
    trade.connectFront("", config.front.addr_trade, config.front.port_trade);
});

trade.onError = safeFunc((data) => {
    logger.error("Trade API Error:", data);
});

trade.onConnected = safeFunc((d) => {
    logger.info("Trade WebSocket connected:", d);
});

trade.onDisconnected = safeFunc((d) => {
    logger.warn("Trade WebSocket disconnected:", d);
});

trade.onFrontConnected = safeFunc((data) => {
    logger.info("Connected to CTP.");
    try {
        trade.set("", config.client.broker_id, config.client.user_id);
        trade.auth("", config.client.user_id, config.client.app_id, config.client.auth_code);
    } catch (e) { 
        logger.error('Trade.set/auth error:', e); 
    }
});

trade.onTradingDay = safeFunc((data) => {
    logger.info("Trading day: " + data.info.trading_day);
});

trade.onFrontDisconnected = safeFunc((data) => {
    logger.warn("Disconnected. Message:", data);
});

trade.onAuthenticate = safeFunc((data) => {
    logger.info("Authenticated. Message:", data);
    try { 
        trade.login("", config.client.user_id, config.client.password); 
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
            ensureDir('/var/lib/webctp/archived');
            ensureDir('/var/lib/webctp/record');
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
        trade.querySettlementInfo("", dateStr); 
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
            trade.confirmSettlementInfo("");
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
        md.connect(config.webctp.addr, config.webctp.port); 
    } catch (e) { logger.error('md.connect error:', e); }
});

const instruments: string[] = [];
const SUBSCRIBE_BATCH_SIZE = 500;
const SUBSCRIBE_BATCH_DELAY_MS = 1000;

const subscribeInBatches = (items: string[]) => {
    const batches: string[][] = [];
    for (let i = 0; i < items.length; i += SUBSCRIBE_BATCH_SIZE) {
        batches.push(items.slice(i, i + SUBSCRIBE_BATCH_SIZE));
    }

    batches.forEach((batch, index) => {
        setTimeout(() => {
            batch.forEach((instrumentId) => {
                try {
                    md.subscribe("", [instrumentId]);
                } catch (e) {
                    logger.error('md.subscribe error:', instrumentId, e);
                }
            });
            logger.info(`Subscribed batch ${index + 1}/${batches.length} (${batch.length} instruments)`);
        }, index * SUBSCRIBE_BATCH_DELAY_MS);
    });
};

trade.onQueryInstrument = safeFunc((data) => {
    try {
        if (data.InstrumentID)
            instruments.push(data.InstrumentID);
        if (data.IsLast) {
            subscribeInBatches(instruments);
            instruments.length = 0;
        }
    } catch (e) {
        logger.error('onQueryInstrument error:', e);
    }
});

const stop = () => {
    try {
        trade.logout("", config.client.user_id);
    } catch (e) { logger.error('trade.logout error:', e); }

    try {
        trade.disconnect();
    } catch (e) { logger.error('trade.disconnect error:', e); }
    try {
        md.disconnect();
    } catch (e) { logger.error('md.disconnect error:', e); }

    stopMarketDataFlushLoop();

    log4js.shutdown(() => {
        logger.info('Logger shutdown complete.');
        process.exit(0);
    });
};

const connect = () => {
    try { 
        trade.connect(config.webctp.addr, config.webctp.port); 
    } catch (e) { logger.error('trade.connect error:', e); }
};

process.on('SIGINT', stop);
process.on('SIGTERM', stop);
process.on('exit', () => {
    stopMarketDataFlushLoop();
    log4js.shutdown();
});

startMarketDataFlushLoop();
connect();