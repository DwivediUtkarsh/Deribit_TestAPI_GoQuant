#pragma once
#include <memory>
#include <string>

#include <drogon/WebSocketClient.h>
#include <json/json.h>

class DrogonWebSocket
{
  private:
    std::shared_ptr<drogon::WebSocketClient> ws_client;
    std::string ws_symbol;
    bool is_connected{false};

    static std::string GetFormattedTimestamp();
    void SubscribeToSymbol(const std::string& symbol);
    void HandleMessage(std::string&& msg, const drogon::WebSocketClientPtr& ws_ptr,
                       const drogon::WebSocketMessageType& type);

  public:
    DrogonWebSocket();
    ~DrogonWebSocket();
    void ConnectToServer(const std::string& symbol);
};
