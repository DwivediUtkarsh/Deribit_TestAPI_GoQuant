#include "web_socket_client.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

DrogonWebSocket::DrogonWebSocket() = default;

DrogonWebSocket::~DrogonWebSocket()
{
    if (ws_client && is_connected)
    {
        ws_client->stop();
    }
}

// Function to get the current timestamp in HH:MM:SS.mmm format
std::string DrogonWebSocket::GetFormattedTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto now_time = std::chrono::system_clock::to_time_t(now);
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    struct tm timeinfo;
    char timestamp[20];
    localtime_s(&timeinfo, &now_time);
    std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &timeinfo);

    std::stringstream ss;
    ss << timestamp << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

// Function to connect to the WebSocket server and subscribe to a symbol
void DrogonWebSocket::ConnectToServer(const std::string& symbol)
{
    ws_symbol = symbol;

    try
    {
        std::cout << GetFormattedTimestamp() << " Connecting to Deribit WebSocket...\n";

        const auto req = drogon::HttpRequest::newHttpRequest();
        req->setPath("/ws/api/v2");
        req->setMethod(drogon::Get);

        ws_client = drogon::WebSocketClient::newWebSocketClient("wss://test.deribit.com");

        ws_client->setMessageHandler(
            [this](std::string&& msg, const drogon::WebSocketClientPtr& ws_ptr,
                   const drogon::WebSocketMessageType& type)
            {
                HandleMessage(std::move(msg), ws_ptr, type);
            });

        const drogon::WebSocketRequestCallback callback =
            [this, symbol](const drogon::ReqResult& result, const drogon::HttpResponsePtr& resp,
                           const drogon::WebSocketClientPtr& ws_conn)
        {
            if (result == drogon::ReqResult::Ok)
            {
                is_connected = true;
                std::cout << GetFormattedTimestamp() << " Connected!\n";
                SubscribeToSymbol(symbol);
            }
            else
            {
                std::cerr << GetFormattedTimestamp()
                          << " Failed to connect: " << (resp ? std::to_string(resp->getStatusCode()) : "N/A")
                          << "\n";
            }
        };

        ws_client->connectToServer(req, callback);
    }
    catch (const std::exception& e)
    {
        std::cerr << GetFormattedTimestamp() << " Exception: " << e.what() << "\n";
    }
}

// Function to subscribe to a symbol on the WebSocket server
void DrogonWebSocket::SubscribeToSymbol(const std::string& symbol)
{
    try
    {
        Json::Value msg;
        msg["jsonrpc"] = "2.0";
        msg["method"] = "public/subscribe";
        msg["params"]["channels"] = Json::Value(Json::arrayValue);
        msg["params"]["channels"].append("book." + symbol + ".100ms");
        msg["id"] = 0;

        const Json::StreamWriterBuilder writer;
        const std::string msg_str = Json::writeString(writer, msg);
        const drogon::WebSocketConnectionPtr& ws_conn = ws_client->getConnection();
        ws_conn->send(msg_str);
        std::cout << GetFormattedTimestamp() << " Subscription request sent for: " << symbol << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << GetFormattedTimestamp() << " Exception during subscription: " << e.what() << "\n";
    }
}

// Function to handle incoming messages from the WebSocket server
void DrogonWebSocket::HandleMessage(std::string&& msg, const drogon::WebSocketClientPtr& ws_ptr,
    const drogon::WebSocketMessageType& type)
{
    try
    {
        if (type == drogon::WebSocketMessageType::Text)
        {
            Json::Value json_data;
            const Json::CharReaderBuilder reader_builder;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader_builder, s, &json_data, &errs))
            {
                if (json_data.isMember("params"))
                {
                    const auto& params = json_data["params"];
                    if (params.isMember("channel") && params.isMember("data"))
                    {
                        const std::string channel = params["channel"].asString();
                        const auto& data = params["data"];

                        if (channel.find("book.") != std::string::npos) // Check if it's an order book update
                        {
                            std::cout << GetFormattedTimestamp() << " Order Book Update:\n";
                            if (data.isMember("bids") && data.isMember("asks"))
                            {
                                std::cout << "Bids:\n";
                                for (const auto& bid : data["bids"])
                                {
                                    std::cout << "Price: " << bid[1].asString() << " Size: " << bid[2].asString() << "\n";
                                }
                                std::cout << "Asks:\n";
                                for (const auto& ask : data["asks"])
                                {
                                    std::cout << "Price: " << ask[1].asString() << " Size: " << ask[2].asString() << "\n";
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                std::cerr << GetFormattedTimestamp() << " Failed to parse message: " << errs << "\n";
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << GetFormattedTimestamp() << " Exception processing message: " << e.what() << "\n";
    }
}

