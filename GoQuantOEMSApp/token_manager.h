#pragma once

#include <chrono>
#include <string>

class TokenManager
{
  private:
    std::string m_access_token;
    std::string m_refresh_token;
    std::chrono::system_clock::time_point token_expiry_time;

    static std::string ReadTokenFromFile(const std::string& file_path);

  public:
    TokenManager(const std::string& access_token_file, const std::string& refresh_token_file,
                 const int& expires_in);

    const std::string& GetAccessToken() const;

    bool IsAccessTokenExpired() const;

    bool RefreshAccessToken(const std::string& client_id, const std::string& client_secret);

    void UpdateTokens(const std::string& new_access_token, const std::string& new_refresh_token,
                      const int& expires_in);
};
