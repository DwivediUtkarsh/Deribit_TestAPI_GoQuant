#include "order_manager.h"

#include <cstdio>
#include <iostream>

#include <drogon/drogon.h>

#include "utility_manager.h"

OrderManager::OrderManager(TokenManager& token_manager)
    : m_client(drogon::HttpClient::newHttpClient(BASE_URL)),
    m_token_manager(token_manager),
    m_api_credentials("api_key.txt", "api_secret.txt")
{
}

bool OrderManager::RefreshTokenIfNeeded() const
{
    if (m_token_manager.IsAccessTokenExpired())
    {
        std::cerr << "Access token expired. Refreshing...\n";
        // Refresh the token
        if (!m_token_manager.RefreshAccessToken(m_api_credentials.GetApiKey(),
            m_api_credentials.GetApiSecret()))
        {
            std::cerr << "Failed to refresh token. Cannot place order.\n";
            return false;
        }
    }
    return true;
}

// Function to get the string representation of the OrderType enum
std::string OrderManager::GetOrderTypeString(const OrderType& type)
{
    switch (type)
    {
    case OrderType::LIMIT:
        return "limit";
    case OrderType::MARKET:
        return "market";
    case OrderType::STOP_LIMIT:
        return "stop_limit";
    case OrderType::STOP_MARKET:
        return "stop_market";
    default:
        return "limit";
    }
}

// Function to place an order using the Deribit API
bool OrderManager::PlaceOrder(const OrderParams& params, const std::string& side, std::string& response) const
{
    std::ios_base::sync_with_stdio(false);

    if (!RefreshTokenIfNeeded())
    {
        return false;
    }

    // Use the valid access token
    const std::string access_token = m_token_manager.GetAccessToken();

    // Create the request
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the request parameters
    side == "buy" ? req->setPath("/api/v2/private/buy") : req->setPath("/api/v2/private/sell");

    // Set the request method
    req->setMethod(drogon::Get);

    // Use sprintf for faster string formatting - avoid string concatenation
    char buffer[BUFFER_SIZE];
    int written;

    if (params.type == OrderType::LIMIT)
    {
        written =
            snprintf(buffer, BUFFER_SIZE, "%s?amount=%.6f&instrument_name=%s&label=%s&price=%.2f&type=%s",
                req->getPath().c_str(), params.amount, params.instrument_name.c_str(),
                params.label.c_str(), params.price, GetOrderTypeString(params.type).c_str());
    }
    else if (params.type == OrderType::MARKET)
    {
        written = snprintf(buffer, BUFFER_SIZE, "%s?amount=%.6f&instrument_name=%s&label=%s&type=%s",
            req->getPath().c_str(), params.amount, params.instrument_name.c_str(),
            params.label.c_str(), GetOrderTypeString(params.type).c_str());
    }
    else
    {
        std::cerr << "Unsupported order type.\n";
        return false;
    }

    if (written < 0 || written >= BUFFER_SIZE)
    {
        std::cerr << "Buffer overflow in request formatting\n";
        return false;
    }

    // Set path using string_view to avoid copies
    const std::string path(buffer, written);
    req->setPath(path);
    const std::string m_auth_prefix{ "Bearer " };
    // Get token and set auth header - minimize string operations
    req->addHeader("Authorization", m_auth_prefix + access_token);
    req->addHeader("Content-Type", "application/x-www-form-urlencoded");

    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                std::cout << "Placed Order:\n";
                UtilityManager::DisplayJsonResponse(response);
            }
            else
            {
                std::cout << "4. Failed! Status: " << (http_response ? http_response->getStatusCode() : 0)
                    << '\n';
                std::cout << "Response: " << (http_response ? http_response->body() : "No response body")
                    << '\n';
                response = "Failed to place order";
            }
        });

    return true;
}

// Function to cancel an order using the Deribit API
bool OrderManager::CancelOrder(const std::string& order_id, std::string& response) const
{
    if (!RefreshTokenIfNeeded())
    {
        return false;
    }

    const std::string access_token = m_token_manager.GetAccessToken();
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the request method
    req->setMethod(drogon::Get);

    // Buffer to store the request body
    char buffer[BUFFER_SIZE];

    // Use sprintf to format the order_id parameter
    const int written = snprintf(buffer, BUFFER_SIZE, "/api/v2/private/cancel?order_id=%s", order_id.c_str());

    if (written < 0 || written >= BUFFER_SIZE)
    {
        std::cerr << "Buffer overflow or error in sprintf.\n";
        return false;
    }

    // Set the request body and headers
    req->setPath(std::string(buffer, written));

    // Add the authorization header with the Bearer token
    req->addHeader("Authorization", "Bearer " + access_token);
    req->addHeader("Content-Type", "application/json");

    // Send the request
    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                UtilityManager::DisplayJsonResponse(response);
            }
            else
            {
                std::cerr << "HTTP Status Code: " << (http_response ? http_response->getStatusCode() : 0)
                    << '\n';
                std::cerr << "Response Body: " << (http_response ? http_response->body() : "No response body")
                    << '\n';
                response = "Failed to cancel order";
            }
        });
    return true;
}

// Function to modify an order using the Deribit API
bool OrderManager::ModifyOrder(const std::string& order_id, const double& new_amount, const double& new_price,
    std::string& response) const
{
    if (!RefreshTokenIfNeeded())
    {
        return false;
    }

    const std::string access_token = m_token_manager.GetAccessToken();
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the request method to GET
    req->setMethod(drogon::Get);

    char buffer[BUFFER_SIZE];

    // Use sprintf to format the URL with parameters
    const int written =
        snprintf(buffer, BUFFER_SIZE, "/api/v2/private/edit?order_id=%s&amount=%.6f&price=%.2f",
            order_id.c_str(), new_amount, new_price);

    if (written < 0 || written >= BUFFER_SIZE)
    {
        std::cerr << "Buffer overflow or error in sprintf.\n";
        return false;
    }

    // Set the formatted path
    req->setPath(std::string(buffer, written));

    // Add headers
    req->addHeader("Authorization", "Bearer " + access_token);
    req->addHeader("Content-Type", "application/json");

    // Send the request
    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                std::cout << "Modified Order:\n";
                UtilityManager::DisplayJsonResponse(response);
            }
            else
            {
                std::cerr << "HTTP Status Code: " << (http_response ? http_response->getStatusCode() : 0)
                    << '\n';
                std::cerr << "Response Body: " << (http_response ? http_response->body() : "No response body")
                    << '\n';
                response = "Failed to modify order";
            }
        });
    return true;
}

// Function to get the order book using the Deribit API
bool OrderManager::GetOrderBook(const std::string& instrument_name, std::string& response) const
{
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the HTTP method and path with instrument_name as a query parameter
    req->setMethod(drogon::Get);
    req->setPath("/api/v2/public/get_order_book?instrument_name=" + instrument_name);

    // Send the request
    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                UtilityManager::DisplayOrderBookJson(response);
            }
            else
            {
                std::cerr << "Failed to get Order Book.\n";
                response = "Failed to get Order Book";
            }
        });
    return true;
}

// Function to get the current positions using the Deribit API
bool OrderManager::GetCurrentPositions(const std::string& currency, const std::string& kind,
    std::string& response) const
{
    if (!RefreshTokenIfNeeded())
    {
        return false;
    }

    const std::string access_token = m_token_manager.GetAccessToken();
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the HTTP method and path, including optional parameters for currency and kind
    req->setMethod(drogon::Get);

    // Format the path based on parameters
    std::string path = "/api/v2/private/get_positions?currency=" + currency;
    if (!kind.empty())
    {
        path += "&kind=" + kind;
    }
    req->setPath(path);

    // Add the authorization header with Bearer token
    req->addHeader("Authorization", "Bearer " + access_token);
    req->addHeader("Content-Type", "application/json");

    // Send the request
    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                UtilityManager::DisplayCurrentPositionsJson(response);
            }
            else
            {
                std::cerr << "Failed to get positions.\n";
                response = "Failed to get positions";
            }
        });
    return true;
}

// Function to get the open orders using the Deribit API
bool OrderManager::GetOpenOrders(std::string& response) const
{
    const auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath("/api/v2/private/get_open_orders");
    req->addHeader("Authorization", "Bearer " + m_token_manager.GetAccessToken());
    req->addHeader("Content-Type", "application/json");

    m_client->sendRequest(
        req,
        [&](const drogon::ReqResult& result, const drogon::HttpResponsePtr& http_response)
        {
            if (result == drogon::ReqResult::Ok && http_response->getStatusCode() == drogon::k200OK)
            {
                response = http_response->body();
                std::cout << "Open Orders:\n";
                UtilityManager::DisplayJsonResponse(response);
            }
            else
            {
                std::cerr << "HTTP Status Code: " << (http_response ? http_response->getStatusCode() : 0)
                    << '\n';
                std::cerr << "Response Body: " << (http_response ? http_response->body() : "No response body")
                    << '\n';
                response = "Failed to get open orders";
            }
        });
    return true;
}
