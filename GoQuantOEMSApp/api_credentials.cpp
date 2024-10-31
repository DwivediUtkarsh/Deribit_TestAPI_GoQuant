#include "api_credentials.h"

#include <fstream>
#include <stdexcept>

ApiCredentials::ApiCredentials(const std::string& key_file_path, const std::string& secret_file_path)
{
    m_api_key = ReadFile(key_file_path);
    m_api_secret = ReadFile(secret_file_path);
}

const std::string& ApiCredentials::GetApiKey() const noexcept
{
    return m_api_key;
}

const std::string& ApiCredentials::GetApiSecret() const noexcept
{
    return m_api_secret;
}

// Utility function to read content from a file
std::string ApiCredentials::ReadFile(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file: " + file_path);
    }
    std::string content;
    std::getline(file, content);
    file.close();
    return content;
}
