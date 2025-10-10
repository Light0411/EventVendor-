#pragma once
#include <iostream>
#include <iomanip>   
#include <cstdlib>

namespace Menu {
    inline void displayPenguinLogo(double eventFunds, int eventHype) {
        std::cout << R"(
       .--.  
      |o_o |                EventVendor+
      |:_/ |      A C++ Exhibitor Management System
     //   \ \  Manage TechExpos, CareerFairs, and more!
    (|     | )   
    /'\_   _/`\
    \___)=(___/
    )" << "\n";

        std::cout << "---------------------------------------------------\n";
        std::cout << " Funds: $" << std::fixed << std::setprecision(2) << eventFunds
            << "    Hype: " << eventHype << "\n";
        std::cout << "---------------------------------------------------\n\n";
    }


    inline void displayMainMenu(double eventFunds, int eventHype) {
        system("cls");
        displayPenguinLogo(eventFunds, eventHype);
        std::cout << "=========== EXHIBITOR MANAGEMENT SYSTEM ===========\n";
        std::cout << "  1. Manage Vendors\n";
        std::cout << "  2. Manage Booths & Logistics\n";
        std::cout << "  3. Manage Products\n";
        std::cout << "  4. Launch Marketing & Promotions\n";
        std::cout << "  5. Manage Transactions\n";
        std::cout << "  6. Manage Feedbacks\n";
        std::cout << "  7. Report Overview\n";
        std::cout << "  8. Save & Exit\n";
        std::cout << "===================================================\n";
        std::cout << "Enter your choice: ";
    }
}
