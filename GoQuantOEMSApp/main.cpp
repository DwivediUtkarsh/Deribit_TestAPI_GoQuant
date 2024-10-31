#include <chrono>
#include <csignal>
#include <iostream>
#include <drogon/drogon.h>
#include "order_manager.h"
#include "utility_manager.h"
#include "web_socket_client.h"

void DisplayMenu()
{
    std::cout << "\nSelect an action:\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Modify Order\n";
    std::cout << "3. Cancel Order\n";
    std::cout << "4. Get Order Book\n";
    std::cout << "5. View Current Positions\n";
    std::cout << "6. Get Open Orders\n";
    std::cout << "7. Connect to WebSocket and Subscribe\n";
    std::cout << "8. Execute\n";
    std::cout << "Enter your choice: ";
}

int main()
{
    std::signal(SIGINT, UtilityManager::HandleExitSignal);
    std::ios_base::sync_with_stdio(false);

    try
    {
        TokenManager token_manager("access_token.txt", "refresh_token.txt", 2505599);
        OrderManager order_manager(token_manager);
        std::string response;

        // Initialize WebSocket client
        auto ws_client = std::make_unique<DrogonWebSocket>();
       

        
        while (true)
        {
            DisplayMenu();
            int choice;
            std::cin >> choice;

            if (choice == 8) break;

            switch (choice)
            {
            case 1:
            {
                // Place Order
                std::string instrument, client_order_id, order_type_str;
                double amount, price;
                int order_type;

                std::cout << "Enter instrument name (e.g., ETH-PERPETUAL): ";
                std::cin >> instrument;
                std::cout << "Enter amount: ";
                std::cin >> amount;
                std::cout << "Enter price: ";
                std::cin >> price;
                std::cout << "Enter client order ID: ";
                std::cin >> client_order_id;
                std::cout << "Enter order type (1 for LIMIT, 2 for MARKET): ";
                std::cin >> order_type;

                OrderType type = (order_type == 1) ? OrderType::LIMIT : OrderType::MARKET;
                OrderParams params{ instrument, amount, price, client_order_id, type };

                std::string side;
                std::cout << "Enter side (buy/sell): ";
                std::cin >> side;

                order_manager.PlaceOrder(params, side, response);
                std::cout << "Response: " << response << '\n';
                break;
            }
            case 2:
            {
                // Modify Order
                std::string order_id;
                double new_amount, new_price;

                std::cout << "Enter order ID to modify: ";
                std::cin >> order_id;
                std::cout << "Enter new amount: ";
                std::cin >> new_amount;
                std::cout << "Enter new price: ";
                std::cin >> new_price;

                order_manager.ModifyOrder(order_id, new_amount, new_price, response);
                std::cout << "Response: " << response << '\n';
                break;
            }
            case 3:
            {
                // Cancel Order
                std::string order_id;

                std::cout << "Enter order ID to cancel: ";
                std::cin >> order_id;

                order_manager.CancelOrder(order_id, response);
                std::cout << "Response: " << response << '\n';
                break;
            }
            case 4:
            {
                // Get Order Book
                std::string instrument;

                std::cout << "Enter instrument name for order book (e.g., ETH-PERPETUAL): ";
                std::cin >> instrument;

                order_manager.GetOrderBook(instrument, response);
                std::cout << "Order Book: " << response << '\n';
                break;
            }
            case 5:
            {
                // View Current Positions
                std::string instrument, instrument_type;

                std::cout << "Enter instrument name (e.g., ETH): ";
                std::cin >> instrument;
                std::cout << "Enter instrument type (e.g., future): ";
                std::cin >> instrument_type;

                order_manager.GetCurrentPositions(instrument, instrument_type, response);
                std::cout << "Current Positions: " << response << '\n';
                break;
            }
            case 6:
            {
                // Get Open Orders
                order_manager.GetOpenOrders(response);
                std::cout << "Open Orders: " << response << '\n';
                break;
            }
            case 7:
            {
                // Connect to WebSocket and Subscribe
                std::string symbol;
                std::cout << "Enter symbol to subscribe (e.g., ETH-PERPETUAL): ";
                std::cin >> symbol;

                ws_client->ConnectToServer(symbol);
                std::cout << "Connected to WebSocket and subscribed to " << symbol << ".\n";
                break;
            }
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
        }

        drogon::app().run();
        std::cout << "Press Enter to exit...";
        std::cin.get();  // Pause before exiting
        std::cin.get();  // Capture the newline after user choice input


    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}