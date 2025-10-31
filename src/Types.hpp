#ifndef TABXX_TYPES_HPP_
#define TABXX_TYPES_HPP_

#include <memory>

#include <uWebSockets/App.h>
#include <uWebSockets/HttpResponse.h>
#include <json.hpp>

namespace tabxx {

class MarketDataHandler;
class TraderHandler;

struct WSContext {
    std::unique_ptr<MarketDataHandler> md;
    std::unique_ptr<TraderHandler> trade;
};

using WebSocket = uWS::WebSocket<false, true, WSContext>;
using HttpResponse = uWS::HttpResponse<false>;
using HttpRequest = uWS::HttpRequest;
using nlohmann::json;

} // namespace tabxx

#endif // TABXX_TYPES_HPP_