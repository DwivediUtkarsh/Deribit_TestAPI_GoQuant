cmake_minimum_required(VERSION 3.10)
project(GoQuantOEMS)

set(CMAKE_CXX_STANDARD 17)

# Set VCPKG
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET "x64-windows")

# Find required packages
find_package(Drogon REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(JsonCpp CONFIG REQUIRED)

# Add executable
add_executable(goquant_oems GoQuantOEMSApp/main.cpp GoQuantOEMSApp/web_socket_client.cpp GoQuantOEMSApp/order_manager.cpp GoQuantOEMSApp/utility_manager.cpp GoQuantOEMSApp/token_manager.cpp GoQuantOEMSApp/api_credentials.cpp ) 

# Link libraries
target_link_libraries(goquant_oems Drogon::Drogon ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES} JsonCpp::JsonCpp)
