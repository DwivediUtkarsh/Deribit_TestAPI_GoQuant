# GoQuant - Order Execution and Management System (OEMS) using Deribit API

## Overview

**GoQuant OEMS** is a cutting-edge C++17 application created to interface with the Deribit cryptocurrency trading platform. It offers an array of trading functions, such as placing, modifying, and canceling orders, along with managing open orders and positions, and fetching order books. Additionally, it features a WebSocket server to allow clients to subscribe to specific symbols and receive continuous updates on the order book.


## Features

- **Place Orders:** Place market and limit orders on Deribit.
- **Modify Orders:** Update existing orders with new quantities or prices.
- **Cancel Orders:** Cancel open orders by order ID.
- **Retrieve Order Book:** Fetch and display the order book for specific trading pairs.
- **View Current Positions:** Display current open positions.
- **WebSocket Server:** Allows clients to subscribe to symbols and receive real-time order book updates.
- **Supported Markets:** Spot, futures, and options for all supported symbols.

## Scope

- Supports **Spot, Futures, and Options** markets.
- Provides low-latency access to **all supported symbols** on Deribit.
- **Websocket server** that clients can connect to and subscribe to a symbol by sending a message.

## Prerequisites

- **C++ Compiler:** C++17 or later.
- **CMake:** Version 3.10 or later.
- **Visual Studio 2022:** or another compatible C++ IDE.
- **vcpkg:** A package manager for C++ libraries.

## Dependencies

- **Drogon:** For HTTP and WebSocket client/server functionality.
- **cURL:** For HTTP requests.
- **OpenSSL:** For handling secure connections.
- **JsonCpp:** For JSON parsing.

## Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/DwivediUtkarsh/Deribit_TestAPI_GoQuant.git
cd Deribit_TestAPI_GoQuant
```

### Step 2: Install Dependencies

1. Install `vcpkg` (if not already installed):
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```
2. Install required packages:

```bash
.\vcpkg install drogon curl openssl jsoncpp
```

### Step 3: Set Up Environment Variables
Create a .env file in the project root directory with the following content:
```bash
API_KEY=your_api_key
SECRET_KEY=your_secret_key
```
Replace `your_api_key` and `your_secret_key` with your Deribit API credentials.

### Step 4: Configure CMake
Create a `CMakeLists.txt` file with the following content:
```bash
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
```

### Step 5: Build the Project
Open the project folder in Visual Studio 2022.
Open `CMakeLists.txt` in Visual Studio.
Right-click on the `CMakeLists.txt` file in the Solution Explorer and choose `Build`.

### Step 6: Run the Project
Run the `goquant_oems` executable generated by the build process.

## Usage

## Example Input Orders for Each Action

Here are example input orders to use with each of the choices available in the program:

1. **Place Order**
   - **Action**: `Buy`
   - **Instrument Name**: `ETH-PERPETUAL`
   - **Amount**: `1`
   - **Price**: `2500.0`
   - **Order Type**: `LIMIT`
   - **Client id**: `MyFirstOrder`

2. **Cancel Order**
   - **Order ID**: `1234-5678-ABCD`

3. **Modify Order**
   - **Order ID**: `1234-5678-ABCD`
   - **New Amount**: `0.2`
   - **New Price**: `45500.0`

4. **Get Order Book**
   - **Instrument Name**: `BTC-PERPETUAL`

5. **Get Current Positions**
   - **Currency**: `BTC`
   - **Kind**: `future`

6. **Get Open Orders**
   - No additional inputs required

7. **Connect to WebSocket and Subscribe
   - **Symbol**: `ETH-PERPETUAL`
     
## Environment Variables
- API_KEY: Your Deribit API key.
- SECRET_KEY: Your Deribit API secret key.

## Error Handling
The application handles various errors, such as failed order placements or WebSocket reconnections. Errors are logged and printed to the console.
In case of error Failed to find access_token.txt, clone the repository again, and from the "Deribit_TestAPI_GoQuant\out\build\x64-Debug",copy and store the access_token,refresh_token ,api_key and api_secret texts files, and after reconfiguration of CMake ,paste them again. (As i am giving this assignment for review I am providing my api_key and api_secret for testing)

