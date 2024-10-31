#pragma once

#include <memory>
#include <string>

#include <drogon/HttpClient.h>

#include "api_credentials.h"
#include "token_manager.h"

enum class OrderType
{
    LIMIT,
    MARKET,
    STOP_LIMIT,
    STOP_MARKET
};

enum class InstrumentType
{
    SPOT,
    FUTURES,
    OPTION
};

struct OrderParams
{
    std::string instrument_name;  // e.g., "BTC-PERPETUAL", "BTC-28JUN24"
    double amount;                // Amount in base currency
    double price;                 // Optional for market orders
    std::string label;            // Client order ID
    OrderType type;               // Order type
    std::string time_in_force;    // "good_til_cancelled", "fill_or_kill" "immediate_or_cancel"
};

class OrderManager
{
  private:
    static constexpr size_t BUFFER_SIZE = 2048;
    static constexpr const char* BASE_URL = "https://test.deribit.com";
    static constexpr const char* API_PATH = "/api/v2/private/";
    std::shared_ptr<drogon::HttpClient> m_client;
    TokenManager& m_token_manager;
    ApiCredentials m_api_credentials;

  public:
    OrderManager(TokenManager& token_manager);
    bool RefreshTokenIfNeeded() const;

    static std::string GetOrderTypeString(const OrderType& type);

    bool PlaceOrder(const OrderParams& params, const std::string& side, std::string& response) const;
    bool CancelOrder(const std::string& order_id, std::string& response) const;
    bool ModifyOrder(const std::string& order_id, const double& new_amount, const double& new_price,
                     std::string& response) const;
    bool GetOrderBook(const std::string& instrument_name, std::string& response) const;
    bool GetCurrentPositions(const std::string& currency, const std::string& kind,
                             std::string& response) const;
    bool GetOpenOrders(std::string& response) const;
};
