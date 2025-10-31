#ifndef TABXX_TRADER_MESSAGE_HANDLER_HPP_
#define TABXX_TRADER_MESSAGE_HANDLER_HPP_

#include <string>

#include <uWebSockets/App.h>
#include <json.hpp>

#include "MarketData/Handler.hpp"
#include "Trade/Handler.hpp"

namespace tabxx {
    
std::string HandleTraderMessage(const nlohmann::json& msg, TraderHandler& trader);

std::string HandleMarketDataMessage(const nlohmann::json&, MarketDataHandler&);

} // namespace tabxx

#endif // TABXX_TRADER_MESSAGE_HANDLER_HPP_