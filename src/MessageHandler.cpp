#include <functional>
#include <unordered_map>

#include "MessageHandler.hpp"

namespace tabxx {

using std::string;
using cjr = const json&;
using mdr = MarketDataHandler&;
using thr = TraderHandler&;

const std::unordered_map<std::string, std::function<std::string(cjr, mdr)>> map_md {
    {"connect", [](cjr j, mdr md) {
        if (!j.contains("addr"))
            return "Error: Field \"addr\" not found.";
        if (!j["addr"].is_string())
            return "Error: Field \"addr\" type error (expected string).";
        if (!j.contains("port"))
            return "Error: Field \"port\" not found.";
        if (!j["port"].is_string())
            return "Error: Field \"port\" type error (expected string).";
        std::string addr = "tcp://" + string(j["addr"]) + ":" + string(j["port"]);
        md.connect(addr);
        return "";
    }},
    {"login", [](cjr j, mdr md) {
        if (!j.contains("broker_id"))
            return "Error: Field \"broker_id\" not found.";
        if (!j["broker_id"].is_string())
            return "Error: Field \"broker_id\" type error (expected string).";
        if (!j.contains("user_id"))
            return "Error: Field \"user_id\" not found.";
        if (!j["user_id"].is_string())
            return "Error: Field \"user_id\" type error (expected string).";
        if (!j.contains("password"))
            return "Error: Field \"password\" not found.";
        if (!j["password"].is_string())
            return "Error: Field \"password\" type error (expected string).";
        md.login(string(j["broker_id"]).c_str(), string(j["user_id"]).c_str(), string(j["password"]).c_str());
        return "";
    }},
    {"logout", [](cjr j, mdr md) {
        if (!j.contains("broker_id"))
            return "Error: Field \"broker_id\" not found.";
        if (!j["broker_id"].is_string())
            return "Error: Field \"broker_id\" type error (expected string).";
        if (!j.contains("user_id"))
            return "Error: Field \"user_id\" not found.";
        if (!j["user_id"].is_string())
            return "Error: Field \"user_id\" type error (expected string).";
        md.logout(j["user_id"], j["broker_id"]);
        return "";
    }},
    {"subscribe", [](cjr j, mdr md) {
        if (!j.contains("instruments"))
            return "Error: Field \"instruments\" not found.";
        if (!j["instruments"].is_array())
            return "Error: \"instruments\" is not an array.";
        md.subscribe(j["instruments"]);
        return "";
    }},
    {"unsubscribe", [](cjr j, mdr md) {
        if (!j.contains("instruments"))
            return "Error: Field \"instruments\" not found.";
        if (!j["instruments"].is_array())
            return "Error: \"instruments\" is not an array.";
        md.unsubscribe(j["instruments"]);
        return "";
    }},
    {"get_trading_day", [](cjr j, mdr md){
        md.getTradingDay();
        return "";
    }}
};

string HandleMarketDataMessage(cjr j, mdr md) {
    if (!j.contains("op"))
        return "Error: Field \"op\" not found.";
    string operation = j["op"];
    if (map_md.find(operation) == map_md.end())
        return "Error: Unknown op \"" + operation + "\".";
    return map_md.at(operation)(j["data"], md);
}

const std::unordered_map<std::string, std::function<std::string(cjr, thr)>> map_trader {
    {"connect", [](cjr j, thr t) {
        t.connect(j["addr"], j["port"]);
        return "";
    }},
    {"get_trading_day", [](cjr j, thr t) {
        t.getTradingDay();
        return "";
    }},
    {"set", [](cjr j, thr t) {
        if (j.contains("broker_id"))
            t.setBrokerID(j["broker_id"]);
        if (j.contains("investor_id"))
            t.setInvestorID(j["investor_id"]);
        return "";
    }},
    {"auth", [](cjr j, thr t) {
        t.auth(j["user_id"], j["app_id"], j["auth_code"]);
        return "";
    }},
    {"login", [](cjr j, thr t) {
        t.login(j["user_id"], j["password"]);
        return "";
    }},
    {"logout", [](cjr j, thr t) {
        t.logout(j["user_id"]);
        return "";
    }},
    {"query_settlement_info", [](cjr j, thr t) {
        t.querySettlementInfo(j["trading_day"]);
        return "";
    }},
    {"confirm_settlement_info", [](cjr j, thr t) {
        t.confirmSettlementInfo();
        return "";
    }},
    {"query_trading_account", [](cjr j, thr t) {
        t.queryTradingAccount();
        return "";
    }},
    {"insert_order", [](cjr j, thr t) {
        try {
            t.insertOrder(
                j["instrument"], j["exchange"], j["ref"], 
                j["price"], j["direction"], j["offset"], j["volume"],
                j["price_type"], j["time_condition"]);
            return "";
        }
        catch (const std::exception& e) {
            return e.what();
        }
    }},
    {"query_order", [](cjr j, thr t) {
        if (j.contains("order_sys_id"))
            t.queryOrderByID(j["order_sys_id"]);
        else if (j.contains("exchange_id"))
            t.queryOrderByExchange(j["exchange_id"]);
        else if (j.contains("from") && j.contains("to"))
            t.queryOrderByRange(j["from"], j["to"]);
        else
            t.queryOrder();
        return "";
    }}
};

string HandleTraderMessage(const json& msg, TraderHandler& trader) {
    if (!msg.contains("op"))
        return "Error: Field \"op\" not found.";
    string operation = msg["op"];
    if (!msg.contains("data"))
        return "Error: Field \"data\" not found.";
    if (map_trader.find(operation) != map_trader.end())
        return map_trader.at(operation)(msg["data"], trader);
    else
        return "Error: Unknown operation \"" + operation + "\".";
    return "";
}

} // namespace tabxx
