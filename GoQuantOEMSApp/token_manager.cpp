#include "token_manager.h"

#include <chrono>
#include <fstream>
#include <iostream>

#include <drogon/HttpClient.h>

std::string TokenManager::ReadTokenFromFile(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::string token;
    std::getline(file, token);
    return token;
}

TokenManager::TokenManager(const std::string& access_token_file, const std::string& refresh_token_file,
                           const int& expires_in)
{
    // Read tokens from the provided files
    m_access_token = ReadTokenFromFile(access_token_file);
    m_refresh_token = ReadTokenFromFile(refresh_token_file);

    // Set the expiry time based on the expires_in value (in seconds)
    token_expiry_time = std::chrono::system_clock::now() + std::chrono::seconds(expires_in);
}

// Function to return the access token
const std::string& TokenManager::GetAccessToken() const
{
    return m_access_token;
}

// Function to check if the access token has expired
bool TokenManager::IsAccessTokenExpired() const
{
    return std::chrono::system_clock::now() >= token_expiry_time;
}

// Function to refresh the access token using the refresh token
bool TokenManager::RefreshAccessToken(const std::string& client_id, const std::string& client_secret)
{
    std::cout << "Refreshing access token using refresh token...\n";

    const auto client = drogon::HttpClient::newHttpClient("https://test.deribit.com");
    const auto req = drogon::HttpRequest::newHttpRequest();

    // Set the request parameters
    req->setMethod(drogon::Post);
    req->setPath("/api/v2/public/auth");
    req->addHeader("Content-Type", "application/x-www-form-urlencoded");

    const std::string body = "grant_type=refresh_token&refresh_token=" + m_refresh_token +
                             "&client_id=" + client_id + "&client_secret=" + client_secret;
    req->setBody(body);

    // Send the request
    auto [result, response] = client->sendRequest(req);
    if (result == drogon::ReqResult::Ok && response->getStatusCode() == drogon::k200OK)
    {
        if (const auto json_resp = response->getJsonObject())
        {
            m_access_token = (*json_resp)["result"]["access_token"].asString();
            m_refresh_token = (*json_resp)["result"]["refresh_token"].asString();
            const int expires_in = (*json_resp)["result"]["expires_in"].asInt();

            token_expiry_time = std::chrono::system_clock::now() + std::chrono::seconds(expires_in);
            std::cout << "Token refreshed successfully!\n";
            return true;
        }
    }

    std::cerr << "Failed to refresh the access token.\n";
    return false;
}

// Function to update the access and refresh tokens
void TokenManager::UpdateTokens(const std::string& new_access_token, const std::string& new_refresh_token,
                                const int& expires_in)
{
    m_access_token = new_access_token;
    m_refresh_token = new_refresh_token;
    token_expiry_time = std::chrono::system_clock::now() + std::chrono::seconds(expires_in);
}
