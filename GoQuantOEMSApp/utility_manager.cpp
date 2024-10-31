#include "utility_manager.h"

#include <drogon/HttpAppFramework.h>

// Signal handler function
void UtilityManager::HandleExitSignal(const int signal)
{
    std::cout << "Exit signal received: " << signal << ". Shutting down Drogon..."
              << "\n";
    drogon::app().quit();  // Stop Drogon's event loop
}

void UtilityManager::DisplayJsonResponse(const std::string& response)
{
    Json::Value json_data;
    if (!IsParseJsonGood(response, json_data))
    {
        return;
    }

    // Check for 'result' member and determine the type of response
    if (json_data.isMember("result"))
    {
        const Json::Value& result = json_data["result"];

        // Handle PlaceOrder or ModifyOrder response
        if (result.isObject() && result.isMember("order"))
        {
            const Json::Value& order = result["order"];
            std::cout << "Order ID: " << order["order_id"].asString() << ", "
                      << "Instrument: " << order["instrument_name"].asString() << ", "
                      << "Type: " << order["order_type"].asString() << ", "
                      << "State: " << order["order_state"].asString() << ", "
                      << "Direction: " << order["direction"].asString() << ", "
                      << "Amount: " << order["amount"].asDouble() << ", "
                      << "Price: " << order["price"].asDouble() << ", "
                      << "Time in Force: " << order["time_in_force"].asString() << ", "
                      << "Creation UTC Timestamp: "
                      << DisplayFormattedTimestamp(order["creation_timestamp"].asInt64()) << "\n\n";
        }
        // Handle CancelOrder response
        else if (result.isObject() && result.isMember("order_id"))
        {
            std::cout << "Cancelled Order ID: " << result["order_id"].asString() << "\n\n";
        }
        // Handle GetOpenOrders response
        else if (result.isArray())
        {
            std::cout << "Number of Open Orders: " << result.size() << "\n";

            for (const auto& order : result)
            {
                std::cout << "Order ID: " << order["order_id"].asString() << ", "
                          << "Instrument: " << order["instrument_name"].asString() << ", "
                          << "Type: " << order["order_type"].asString() << ", "
                          << "State: " << order["order_state"].asString() << ", "
                          << "Direction: " << order["direction"].asString() << ", "
                          << "Amount: " << order["amount"].asDouble() << ", "
                          << "Filled Amount: " << order["filled_amount"].asDouble() << ", "
                          << "Price: " << order["price"].asDouble() << ", "
                          << "Time in Force: " << order["time_in_force"].asString() << ", "
                          << "Creation UTC Timestamp: "
                          << DisplayFormattedTimestamp(order["creation_timestamp"].asInt64()) << "\n";
            }
        }
        // Handle other types of responses, if needed
        else
        {
            std::cout << "Unhandled JSON structure in result.\n";
        }
    }
    else
    {
        std::cerr << "Unexpected JSON structure. 'result' field not found.\n";
    }
}

std::string UtilityManager::DisplayFormattedTimestamp(const int64_t& timestamp_ms)
{
    std::stringstream time;
    const std::time_t timestamp_sec = timestamp_ms / 1000;
    std::tm tm_time;
    if (gmtime_s(&tm_time, &timestamp_sec) != 0)
    {
        return "Invalid timestamp";  // Handle error if conversion fails
    }
    time << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");

    return time.str();
}

void UtilityManager::DisplayCurrentPositionsJson(const std::string& response)
{
    Json::Value json_data;
    if (!IsParseJsonGood(response, json_data))
    {
        return;
    }

    // Check for 'result' member and ensure it is an array for positions data
    if (json_data.isMember("result") && json_data["result"].isArray())
    {
        const Json::Value& positions = json_data["result"];
        std::cout << "Current Positions:\n";
        for (const auto& position : positions)
        {
            std::cout << "Instrument: " << position["instrument_name"].asString() << ", "
                      << "Direction: " << position["direction"].asString() << ", "
                      << "Size: " << position["size"].asDouble() << ", "
                      << "Mark Price: " << position["mark_price"].asDouble() << ", "
                      << "Average Price: " << position["average_price"].asDouble() << ", "
                      << "Floating P&L: " << position["floating_profit_loss"].asDouble() << ", "
                      << "Total P&L: " << position["total_profit_loss"].asDouble() << ", "
                      << "Leverage: " << position["leverage"].asDouble() << ", "
                      << "Maintenance Margin: " << position["maintenance_margin"].asDouble() << ", "
                      << "Initial Margin: " << position["initial_margin"].asDouble() << ", "
                      << "Open Orders Margin: " << position["open_orders_margin"].asDouble() << ", "
                      << "Timestamp: " << DisplayFormattedTimestamp(position["creation_timestamp"].asInt64())
                      << "\n\n";
        }
    }
    else
    {
        std::cerr << "Unexpected JSON structure for positions data.\n";
    }
}

void UtilityManager::DisplayOrderBookJson(const std::string& response)
{
    Json::Value json_data;
    if (!IsParseJsonGood(response, json_data))
    {
        return;
    }

    // Check for 'result' member to verify the structure of the order book data
    if (json_data.isMember("result"))
    {
        const Json::Value& result = json_data["result"];
        std::cout << "Order Book:\n";

        // Displaying general order book info
        std::cout << "Instrument: " << result["instrument_name"].asString() << "\n"
                  << "Best Bid Price: " << result["best_bid_price"].asDouble() << ", "
                  << "Best Ask Price: " << result["best_ask_price"].asDouble() << "\n"
                  << "Mark Price: " << result["mark_price"].asDouble() << "\n"
                  << "Index Price: " << result["index_price"].asDouble() << "\n";

        // Display Bids
        if (result.isMember("bids") && result["bids"].isArray())
        {
            std::cout << "\nBids:\n";
            for (const auto& bid : result["bids"])
            {
                std::cout << "Price: " << bid[0].asDouble() << ", "
                          << "Amount: " << bid[1].asDouble() << "\n";
            }
        }

        // Display Asks
        if (result.isMember("asks") && result["asks"].isArray())
        {
            std::cout << "\nAsks:\n";
            for (const auto& ask : result["asks"])
            {
                std::cout << "Price: " << ask[0].asDouble() << ", "
                          << "Amount: " << ask[1].asDouble() << "\n";
            }
        }
    }
    else
    {
        std::cerr << "Unexpected JSON structure for order book data.\n";
    }
    std::cout << "\n";
}

bool UtilityManager::IsParseJsonGood(const std::string& response, Json::Value& json_data)
{
    const Json::CharReaderBuilder reader_builder;
    std::string errs;

    std::istringstream s(response);
    if (!Json::parseFromStream(reader_builder, s, &json_data, &errs))
    {
        std::cerr << "Failed to parse JSON: " << errs << "\n";
        return false;
    }

    // Check if there's an error in the response
    if (json_data.isMember("error"))
    {
        std::cerr << "Error: " << json_data["error"]["message"].asString() << ", "
                  << "Code: " << json_data["error"]["code"].asInt() << "\n";
        return false;
    }
    return true;
}
