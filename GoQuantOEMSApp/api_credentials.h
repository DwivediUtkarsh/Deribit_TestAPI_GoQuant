#pragma once

#include <string>

class ApiCredentials
{
  private:
    std::string m_api_key;
    std::string m_api_secret;

  public:
    ApiCredentials(const std::string& key_file_path, const std::string& secret_file_path);

    const std::string& GetApiKey() const noexcept;
    const std::string& GetApiSecret() const noexcept;
    std::string ReadFile(const std::string& file_path);
};
