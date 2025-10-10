#pragma once

#include <iostream>
#include <string>
#include <limits>
#include <windows.h>
#include <vector>      // Added for daysInMonth array
#include <regex>       // Added for date format validation
#include <chrono>      // Added for system date checking
#include <sstream>     // Added for parsing date components
#include <ctime>       // Added for time functions

// 'inline' prevents linker errors for functions defined in a header.

inline void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

inline int getSubMenuChoice(const std::string& moduleName) {
    system("cls");
    std::cout << "\n--- Manage " << moduleName << " ---\n";
    std::cout << "1. Add New " << moduleName << "\n";
    std::cout << "2. View All " << moduleName << "\n";
    std::cout << "3. Update " << moduleName << "\n";
    std::cout << "4. Delete " << moduleName << "\n";
    std::cout << "0. Return to Main Menu\n";
    std::cout << "-----------------------\n";
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return choice;
}

inline bool isValidDate(const std::string& dateStr, bool allowPastDates = false) {
    // 1. Check format YYYY-MM-DD
    const std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!std::regex_match(dateStr, pattern)) {
        std::cout << "Error: Invalid format. Please use YYYY-MM-DD.\n";
        return false;
    }

    // 2. Check for valid month and day values
    int year, month, day;
    char dash1, dash2;
    std::stringstream ss(dateStr);
    ss >> year >> dash1 >> month >> dash2 >> day;

    if (month < 1 || month > 12) {
        std::cout << "Error: Month must be between 01 and 12.\n";
        return false;
    }

    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Check for leap year
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    if (day < 1 || day > daysInMonth[month]) {
        std::cout << "Error: Invalid day (" << day << ") for month " << month << ".\n";
        return false;
    }

    // 3. Check if date is in the past 
    if (!allowPastDates) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm;
        localtime_s(&now_tm, &now_time_t); // Use safer `localtime_s`

        if (year < (now_tm.tm_year + 1900)) {
            std::cout << "Error: Date cannot be in the past.\n";
            return false;
        }
        if (year == (now_tm.tm_year + 1900) && month < (now_tm.tm_mon + 1)) {
            std::cout << "Error: Date cannot be in the past.\n";
            return false;
        }
        if (year == (now_tm.tm_year + 1900) && month == (now_tm.tm_mon + 1) && day < now_tm.tm_mday) {
            std::cout << "Error: Date cannot be in the past.\n";
            return false;
        }
    }
    return true;
}