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
        md.connect(string(j["addr"]), string(j["port"]));
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
        // if (!j.contains("broker_id"))
        //     return "Error: Field \"broker_id\" not found.";
        // if (!j["broker_id"].is_string())
        //     return "Error: Field \"broker_id\" type error (expected string).";
        // if (!j.contains("user_id"))
        //     return "Error: Field \"user_id\" not found.";
        // if (!j["user_id"].is_string())
        //     return "Error: Field \"user_id\" type error (expected string).";
        // md.logout(j["user_id"], j["broker_id"]);
        return "Unsupported method";
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
        return "Error: Field `op` not found.";
    string operation = j["op"];
    if (map_md.find(operation) == map_md.end())
        return "Error: Unknown op `" + operation + "`.";
    if (!j.contains("data"))
        return "Error: Field `data` not found.";
    if (!j["data"].is_object())
        return "Error: Field `data` type error (expected object).";
    return map_md.at(operation)(j["data"], md);
}

const std::unordered_map<std::string, std::function<std::string(cjr, thr)>> map_trader {
    {"connect", [](cjr j, thr t) {
        if (!j.contains("addr"))
            return "Error: Field \"addr\" not found.";
        if (!j["addr"].is_string())
            return "Error: Field \"addr\" type error (expected string).";
        if (!j.contains("port"))
            return "Error: Field \"port\" not found.";
        if (!j["port"].is_string())
            return "Error: Field \"port\" type error (expected string).";
        t.connect(j["addr"], j["port"]);
        return "";
    }},
    {"get_trading_day", [](cjr j, thr t) {
        t.getTradingDay();
        return "";
    }},
    {"set", [](cjr j, thr t) {
        if (j.contains("broker_id")) {
            if (j["broker_id"].is_string())
                t.setBrokerID(j["broker_id"]);
            else
                return "Error: Field \"broker_id\" type error (expected string).";
        }
        if (j.contains("investor_id")) {
            if (j["investor_id"].is_string())
                t.setInvestorID(j["investor_id"]);
            else
                return "Error: Field \"investor_id\" type error (expected string).";
        }
        return "";
    }},
    {"auth", [](cjr j, thr t) {
        if (!j.contains("user_id"))
            return "Error: Field \"user_id\" not found.";
        if (!j["user_id"].is_string())
            return "Error: Field \"user_id\" type error (expected string).";
        if (!j.contains("app_id"))
            return "Error: Field \"app_id\" not found.";
        if (!j["app_id"].is_string())
            return "Error: Field \"app_id\" type error (expected string).";
        if (!j.contains("auth_code"))
            return "Error: Field \"auth_code\" not found.";
        if (!j["auth_code"].is_string())
            return "Error: Field \"auth_code\" type error (expected string).";
        t.auth(j["user_id"], j["app_id"], j["auth_code"]);
        return "";
    }},
    {"login", [](cjr j, thr t) {
        if (!j.contains("user_id"))
            return "Error: Field \"user_id\" not found.";
        if (!j["user_id"].is_string())
            return "Error: Field \"user_id\" type error (expected string).";
        if (!j.contains("password"))
            return "Error: Field \"password\" not found.";
        if (!j["password"].is_string())
            return "Error: Field \"password\" type error (expected string).";
        t.login(j["user_id"], j["password"]);
        return "";
    }},
    {"logout", [](cjr j, thr t) {
        if (!j.contains("user_id"))
            return "Error: Field \"user_id\" not found.";
        if (!j["user_id"].is_string())
            return "Error: Field \"user_id\" type error (expected string).";
        t.logout(j["user_id"]);
        return "";
    }},
    {"query_settlement_info", [](cjr j, thr t) {
        if (!j.contains("trading_day"))
            return "Error: Field \"trading_day\" not found.";
        if (!j["trading_day"].is_string())
            return "Error: Field \"trading_day\" type error (expected string).";
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
            if (!j.contains("instrument"))
                return "Error: Field \"instrument\" not found.";
            if (!j["instrument"].is_string())
                return "Error: Field \"instrument\" type error (expected string).";
            if (!j.contains("exchange"))
                return "Error: Field \"exchange\" not found.";
            if (!j["exchange"].is_string())
                return "Error: Field \"exchange\" type error (expected string).";
            if (!j.contains("ref"))
                return "Error: Field \"ref\" not found.";
            if (!j["ref"].is_string())
                return "Error: Field \"ref\" type error (expected string).";
            if (!j.contains("price"))
                return "Error: Field \"price\" not found.";
            if (!j["price"].is_number())
                return "Error: Field \"price\" type error (expected number).";
            if (!j.contains("direction"))
                return "Error: Field \"direction\" not found.";
            if (!j["direction"].is_number_integer())
                return "Error: Field \"direction\" type error (expected integer).";
            if (static_cast<int>(j["direction"]) < 0 || static_cast<int>(j["direction"]) > 1)
                return "Error: Field \"direction\" out of range (0..1).";
            if (!j.contains("offset"))
                return "Error: Field \"offset\" not found.";
            if (!j["offset"].is_number_integer())
                return "Error: Field \"offset\" type error (expected integer).";
            if (static_cast<int>(j["offset"]) < 0 || static_cast<int>(j["offset"]) > 6)
                return "Error: Field \"offset\" out of range (0..6).";
            if (!j.contains("volume"))
                return "Error: Field \"volume\" not found.";
            if (!j["volume"].is_number())
                return "Error: Field \"volume\" type error (expected number).";
            if (!j.contains("price_type"))
                return "Error: Field \"price_type\" not found.";
            if (!j["price_type"].is_number_integer())
                return "Error: Field \"price_type\" type error (expected integer).";
            if (static_cast<int>(j["price_type"]) < 0 || static_cast<int>(j["price_type"]) > 2)
                return "Error: Field \"price_type\" out of range (0..2).";
            if (!j.contains("time_condition"))
                return "Error: Field \"time_condition\" not found.";
            if (!j["time_condition"].is_number_integer())
                return "Error: Field \"time_condition\" type error (expected integer).";
            if (static_cast<int>(j["time_condition"]) < 0 || static_cast<int>(j["time_condition"]) > 1)
                return "Error: Field \"time_condition\" out of range (0..1).";
            if (j.contains("memo")) {
                if (!j["memo"].is_string())
                    return "Error: Field \"memo\" type error (expected string).";
            }
            t.insertOrder(
                j["instrument"], j["exchange"], j["ref"], 
                j["price"], j["direction"], j["offset"], j["volume"],
                j["price_type"], j["time_condition"], j.contains("memo")? j["memo"]: "");
            return "";
        }
        catch (const std::exception& e) {
            return e.what();
        }
    }},
    {"query_order", [](cjr j, thr t) {
        if (j.contains("order_sys_id")) {
            if (!j["order_sys_id"].is_string())
                return "Error: Field \"order_sys_id\" type error (expected string).";
            t.queryOrderByID(j["order_sys_id"]);
        }
        else if (j.contains("exchange_id")) {
            if (!j["exchange_id"].is_string())
                return "Error: Field \"exchange_id\" type error (expected string).";
            t.queryOrderByExchange(j["exchange_id"]);
        }
        else if (j.contains("from") && j.contains("to")) {
            if (!j["from"].is_string())
                return "Error: Field \"from\" type error (expected string).";
            if (!j["to"].is_string())
                return "Error: Field \"to\" type error (expected string).";
            t.queryOrderByRange(j["from"], j["to"]);
        }
        else
            t.queryOrder();
        return "";
    }},
    {"delete_order", [](cjr j, thr t) {
        if (!j.contains("exchange"))
            return "Error: Field `exchange` not found.";
        if (!j["exchange"].is_string())
            return "Error: Field `exchange` type error (expected string).";
        if (!j.contains("instrument"))
            return "Error: Field `instrument` not found.";
        if (!j["instrument"].is_string())
            return "Error: Field `instrument` type error (expected string).";
        if (!j.contains("delete_ref"))
            return "Error: Field `delete_ref` not found.";
        if (!j["delete_ref"].is_number_integer())
            return "Error: Field `delete_ref` type error (expected integer).";
        if (!j.contains("order_sys_id"))
            return "Error: Field `order_sys_id` not found.";
        if (!j["order_sys_id"].is_string())
            return "Error: Field `order_sys_id` type error (expected string).";
        t.deleteOrder(j["exchange"], j["instrument"], j["delete_ref"], j["order_sys_id"]);
        return "";
    }},
    {"query_instrument", [](cjr j, thr t) {
        if (j.contains("exchange")) {
            if (!j["exchange"].is_string())
                return "Error: Field \"exchange\" type error (expected string).";
            if (j.contains("instrument")) {
                if (!j["instrument"].is_string())
                    return "Error: Field \"instrument\" type error (expected string).";
                if (j.contains("exchange_inst_id")) {
                    if (!j["exchange_inst_id"].is_string())
                        return "Error: Field \"exchange_inst_id\" type error (expected string).";
                    if (j.contains("product_id")) {
                        if (!j["product_id"].is_string())
                            return "Error: Field \"product_id\" type error (expected string).";
                        t.queryInstrument(j["exchange"], j["instrument"], j["exchange_inst_id"], j["product_id"]);
                    }
                    else {
                        t.queryInstrument(j["exchange"], j["instrument"], j["exchange_inst_id"]);
                    }
                }
                else {
                    t.queryInstrument(j["exchange"], j["instrument"]);
                }
            }
            else {
                t.queryInstrument(j["exchange"]);
            }
        }
        else {
            t.queryInstrument();
        }
        return "";
    }}
};

string HandleTraderMessage(const json& msg, TraderHandler& trader) {
    if (!msg.contains("op"))
        return "Error: Field `op` not found.";
    string operation = msg["op"];
    if (!msg.contains("data"))
        return "Error: Field `data` not found.";
    if (!msg["data"].is_object())
        return "Error: Field `data` type error (expected object).";
    if (map_trader.find(operation) != map_trader.end())
        return map_trader.at(operation)(msg["data"], trader);
    else
        return "Error: Unknown operation `" + operation + "`.";
    return "";
}

} // namespace tabxx
