#define NOMINMAX
#include "BoothManagement.h"
#include "ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Logistics.h"
#include "../Entity/Booking.h"
#include "../Entity/Booth.h"
#include "../Entity/Vendor.h"
#include "../Entity/Transaction.h"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>
#include <memory>
#include <algorithm>

const int MAP_ROWS = 20;
const int MAP_COLS = 20;

// --- Forward Declarations ---
static void manageLogisticsForBooth(Booth* booth, std::vector<std::unique_ptr<Logistics>>& allLogistics);
static void manageBookingsForFloor(const std::string& mallName, int floorLevel, const std::vector<std::unique_ptr<Booth>>& allBooths, const std::vector<std::unique_ptr<Vendor>>& allVendors, std::vector<std::unique_ptr<Booking>>& allBookings, std::vector<std::unique_ptr<Transaction>>& transactions);
static void manageSingleFloorBooths(
    std::vector<std::unique_ptr<Booth>>& allBooths,
    const std::vector<std::unique_ptr<Vendor>>& allVendors,
    std::vector<std::unique_ptr<Logistics>>& allLogistics,
    std::vector<std::unique_ptr<Booking>>& allBookings,
    std::vector<std::unique_ptr<Transaction>>& transactions,
    const std::string& mallName, int floorLevel);

static std::vector<Booth*> getBoothsForLocation(const std::vector<std::unique_ptr<Booth>>& allBooths, const std::string& mall, int floor) {
    std::vector<Booth*> filteredBooths;
    for (const auto& booth : allBooths) {
        if (booth->mallName == mall && booth->floorLevel == floor) {
            filteredBooths.push_back(booth.get());
        }
    }
    return filteredBooths;
}

static Booth* findBoothByIdInVector(const std::vector<Booth*>& booths, const std::string& id) {
    for (auto* booth : booths) {
        if (booth->boothId == id) { return booth; }
    }
    return nullptr;
}

static void displayBoothLayout(const std::vector<Booth*>& floorBooths, const std::vector<std::unique_ptr<Vendor>>& allVendors, const std::vector<std::unique_ptr<Logistics>>& allLogistics, const std::vector<std::unique_ptr<Booking>>& allBookings) {
    system("cls");
    std::cout << "--- Event Floor Layout ---\n";
    std::cout << "Key: ";
    setConsoleColor(10); std::cout << "[Occupied] ";
    setConsoleColor(14); std::cout << "[Available] ";
    setConsoleColor(7); std::cout << "| *Border* = Premium Booth\n\n";
    const int INNER_WIDTH = 14;
    std::vector<std::vector<std::string>> boothLayout(MAP_ROWS, std::vector<std::string>(MAP_COLS, ""));
    for (const auto* booth : floorBooths) {
        if (booth->row >= 0 && booth->row < MAP_ROWS && booth->col >= 0 && booth->col < MAP_COLS) {
            boothLayout[booth->row][booth->col] = booth->boothId;
        }
    }
    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int lineNum = 1; lineNum <= 5; ++lineNum) {
            for (int c = 0; c < MAP_COLS; ++c) {
                const Booth* booth = findBoothByIdInVector(floorBooths, boothLayout[r][c]);
                bool isOccupied = (booth && booth->assignedVendorId != 0);
                bool isPremium = (booth && booth->type == "Premium");
                if (isOccupied) setConsoleColor(10);
                else if (booth) setConsoleColor(14);
                switch (lineNum) {
                case 1: case 5: {
                    char corner = isPremium ? '*' : '+'; char line = isPremium ? '=' : '-';
                    std::cout << corner << std::string(INNER_WIDTH + 2, line) << corner;
                    break;
                }
                case 2: {
                    char side = isPremium ? '*' : '|';
                    std::stringstream boothText;
                    if (booth) boothText << "Booth " << booth->boothId;
                    std::cout << side << " " << std::left << std::setw(INNER_WIDTH) << boothText.str() << " " << side;
                    break;
                }
                case 3: {
                    char side = isPremium ? '*' : '|';
                    std::cout << side << " ";
                    if (isOccupied) {
                        const Vendor* vendor = GenericUtils::findById(allVendors, booth->assignedVendorId);
                        std::string vName = vendor ? vendor->name : "Unknown";
                        if (vName.length() > INNER_WIDTH) vName = vName.substr(0, INNER_WIDTH);
                        std::cout << std::left << std::setw(INNER_WIDTH) << vName;
                    }
                    else {
                        std::cout << std::left << std::setw(INNER_WIDTH) << (booth ? "Available" : "Empty");
                    }
                    std::cout << " " << side;
                    break;
                }
                case 4: {
                    char side = isPremium ? '*' : '|';
                    std::cout << side << " ";
                    if (isOccupied) {
                        const Booking* currentBooking = nullptr;
                        for (const auto& book : allBookings) {
                            if (book->boothId == booth->boothId && book->vendorId == booth->assignedVendorId) {
                                currentBooking = book.get(); break;
                            }
                        }
                        std::string dateText;
                        if (currentBooking) {
                            dateText = currentBooking->startDate.substr(5) + " to " + currentBooking->endDate.substr(5);
                        }
                        std::cout << std::left << std::setw(INNER_WIDTH) << dateText;
                    }
                    else {
                        std::cout << std::left << std::setw(INNER_WIDTH) << " ";
                    }
                    std::cout << " " << side;
                    break;
                }
                }
                setConsoleColor(7);
                std::cout << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    system("pause");
}

static void manageSingleFloorBooths(
    std::vector<std::unique_ptr<Booth>>& allBooths,
    const std::vector<std::unique_ptr<Vendor>>& allVendors,
    std::vector<std::unique_ptr<Logistics>>& allLogistics,
    std::vector<std::unique_ptr<Booking>>& allBookings,
    std::vector<std::unique_ptr<Transaction>>& transactions,
    const std::string& mallName, int floorLevel)
{
    int choice = -1;
    while (choice != 0) {
        std::vector<Booth*> floorBooths = getBoothsForLocation(allBooths, mallName, floorLevel);
        system("cls");
        std::cout << "\n--- Managing: " << mallName << " - Floor " << floorLevel << " ---\n";
        std::cout << "1. Add New Booth to this Floor\n";
        std::cout << "2. View Booths on this Floor (List)\n";
        std::cout << "3. View Booth Layout for this Floor (Map)\n";
        std::cout << "4. Manage Bookings\n";
        std::cout << "5. Manage Check-In / Check-Out\n";
        std::cout << "6. Manage Logistics for a Booth\n";
        std::cout << "7. Delete a Booth\n";
        std::cout << "0. Return to Location Selection\n";
        std::cout << "-------------------------------------------\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1;
        }
        switch (choice) {
        case 1: {
            system("cls");
            std::cout << "--- Add New Booth to " << mallName << " Floor " << floorLevel << " ---\n";
            int row, col;
            std::string type, id;
            double price;
            std::cout << "Enter Booth ID (e.g., M101, P203, T301): ";
            std::cin >> id;
            char expectedFirstChar = toupper(mallName[0]);
            if (id.empty() || toupper(id[0]) != expectedFirstChar) {
                std::cout << "Error: Booth ID must start with '" << expectedFirstChar << "'.\n";
                system("pause"); break;
            }
            if (GenericUtils::findById(allBooths, id) != nullptr) {
                std::cout << "Error: Duplicate ID across all locations.\n";
                system("pause"); break;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter Booth Type (e.g., Standard, Premium): ";
            getline(std::cin, type);
            std::cout << "Enter Price: ";
            std::cin >> price;

            // ✅ FIX: Validate price
            if (std::cin.fail() || price < 0) {
                std::cout << "Error: Invalid or negative price.\n";
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                system("pause"); break;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter Map Row (0-" << MAP_ROWS - 1 << "): ";
            std::cin >> row;
            std::cout << "Enter Map Column (0-" << MAP_COLS - 1 << "): ";
            std::cin >> col;
            if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) {
                std::cout << "Error: Invalid map coordinates.\n";
                system("pause"); break;
            }
            auto newBooth = std::make_unique<Booth>(id, mallName, floorLevel, type, price);
            newBooth->row = row;
            newBooth->col = col;
            allBooths.push_back(std::move(newBooth));
            std::cout << "Booth added successfully!\n";
            system("pause");
            break;
        }
        case 2: {
            system("cls");
            std::cout << "--- Booths on " << mallName << " - Floor " << floorLevel << " ---\n\n";
            if (floorBooths.empty()) {
                std::cout << "No booths to display for this floor.\n";
            }
            else {
                std::sort(floorBooths.begin(), floorBooths.end(), [](const Booth* a, const Booth* b) {
                    return a->boothId < b->boothId;
                    });
                for (const auto* b : floorBooths) {
                    std::vector<const Logistics*> deliveredLogistics;
                    double totalLogisticsCost = 0.0;
                    for (const auto& log : allLogistics) {
                        if (log->boothId == b->boothId && log->status == "Delivered") {
                            deliveredLogistics.push_back(log.get());
                            totalLogisticsCost += log->getTotalCost();
                        }
                    }
                    const int cardWidth = 26;
                    std::cout << "+============================+\n";
                    std::cout << "| ID:     " << std::left << std::setw(cardWidth - 7) << b->boothId << "|\n";
                    std::cout << "| Type:   " << std::left << std::setw(cardWidth - 7) << b->type << "|\n";
                    std::cout << "| Price:  $" << std::left << std::setw(cardWidth - 8) << std::fixed << std::setprecision(2) << b->price << "|\n";
                    if (b->assignedVendorId == 0) {
                        std::cout << "| Status: ";
                        setConsoleColor(14); std::cout << std::left << std::setw(cardWidth - 7) << "Available";
                        setConsoleColor(7);  std::cout << "|\n";
                    }
                    else {
                        const Vendor* vendor = GenericUtils::findById(allVendors, b->assignedVendorId);
                        std::cout << "| Status: ";
                        setConsoleColor(10); std::cout << std::left << std::setw(cardWidth - 7) << "Occupied";
                        setConsoleColor(7);  std::cout << "|\n";
                        std::cout << "| Vendor: " << std::left << std::setw(cardWidth - 7) << (vendor ? vendor->name : "Unknown") << "|\n";
                        const Booking* currentBooking = nullptr;
                        for (const auto& book : allBookings) {
                            if (book->boothId == b->boothId && book->vendorId == b->assignedVendorId) {
                                currentBooking = book.get(); break;
                            }
                        }
                        if (currentBooking) {
                            std::string duration = currentBooking->startDate.substr(5) + " to " + currentBooking->endDate.substr(5);
                            std::cout << "| Dates:  " << std::left << std::setw(cardWidth - 7) << duration << "|\n";
                        }
                    }
                    std::stringstream coords;
                    coords << "[" << b->row << "][" << b->col << "]";
                    std::cout << "| Coords: " << std::left << std::setw(cardWidth - 7) << coords.str() << "|\n";
                    if (!deliveredLogistics.empty()) {
                        std::cout << "| -------------------------- |\n";
                        std::cout << "| Logistics Acquired:        |\n";
                        for (const auto* log : deliveredLogistics) {
                            std::stringstream itemLine;
                            itemLine << "  - " << log->quantity << "x " << log->itemName;
                            std::string itemStr = itemLine.str();
                            if (itemStr.length() > cardWidth) itemStr = itemStr.substr(0, cardWidth + 0) + ".";
                            std::cout << "| " << std::left << std::setw(cardWidth + 1) << itemStr << "|\n";
                        }
                        std::stringstream costLine;
                        costLine << "Logistics Cost: $" << std::fixed << std::setprecision(2) << totalLogisticsCost;
                        std::cout << "| " << std::left << std::setw(cardWidth + 1) << costLine.str() << "|\n";
                    }
                    std::cout << "+============================+\n\n";
                }
            }
            system("pause");
            break;
        }
        case 3: {
            displayBoothLayout(floorBooths, allVendors, allLogistics, allBookings);
            break;
        }
        case 4: {
            manageBookingsForFloor(mallName, floorLevel, allBooths, allVendors, allBookings, transactions);
            break;
        }
        case 5: {
            system("cls");
            std::cout << "--- Manage Check-In / Check-Out ---\n";
            std::string boothId;
            std::cout << "Enter Booth ID to manage: ";
            std::cin >> boothId;
            Booth* booth = GenericUtils::findById(allBooths, boothId);
            if (booth == nullptr || booth->mallName != mallName || booth->floorLevel != floorLevel) {
                std::cout << "Error: Booth ID not found on this floor.\n";
                system("pause");
                break;
            }
            std::cout << "\n1. Check-In a Vendor\n";
            std::cout << "2. Check-Out current Vendor\n";
            std::cout << "Enter choice: ";
            int statusChoice;
            std::cin >> statusChoice;
            if (statusChoice == 1) {
                std::cout << "Enter Booking ID to check-in: ";
                int bookingId;
                std::cin >> bookingId;
                Booking* bookingToAssign = nullptr;
                for (const auto& b : allBookings) {
                    if (b->bookingId == bookingId) {
                        bookingToAssign = b.get(); break;
                    }
                }
                if (bookingToAssign == nullptr || bookingToAssign->boothId != boothId) {
                    std::cout << "Error: Booking ID not valid for this booth.\n";
                }
                else {
                    booth->assignedVendorId = bookingToAssign->vendorId;
                    std::cout << "Vendor " << booth->assignedVendorId << " successfully checked into booth " << boothId << ".\n";
                }
            }
            else if (statusChoice == 2) {
                booth->assignedVendorId = 0;
                std::cout << "Booth " << boothId << " is now checked-out and marked as available.\n";
            }
            system("pause");
            break;
        }
        case 6: {
            system("cls");
            std::cout << "--- Manage Booth Logistics ---\n";
            std::string boothId;
            std::cout << "Enter Booth ID to manage its logistics: ";
            std::cin >> boothId;
            Booth* booth = GenericUtils::findById(allBooths, boothId);
            if (booth == nullptr || booth->mallName != mallName || booth->floorLevel != floorLevel) {
                std::cout << "Error: Booth not found on this floor.\n";
                system("pause");
            }
            else {
                manageLogisticsForBooth(booth, allLogistics);
            }
            break;
        }
        case 7: {
            system("cls");
            std::cout << "--- Delete Booth ---\n";
            std::string id;
            std::cout << "Enter ID of booth to delete from this floor: ";
            std::cin >> id;
            auto it = std::find_if(allBooths.begin(), allBooths.end(), [id, &mallName, floorLevel](const std::unique_ptr<Booth>& b) {
                return b->boothId == id && b->mallName == mallName && b->floorLevel == floorLevel;
                });
            if (it == allBooths.end()) {
                std::cout << "Error: Booth not found on this floor.\n";
            }
            else {
                allLogistics.erase(std::remove_if(allLogistics.begin(), allLogistics.end(),
                    [id](const std::unique_ptr<Logistics>& log) { return log->boothId == id; }), allLogistics.end());
                std::cout << "Associated logistics cleared.\n";
                allBookings.erase(std::remove_if(allBookings.begin(), allBookings.end(),
                    [id](const std::unique_ptr<Booking>& book) { return book->boothId == id; }), allBookings.end());
                std::cout << "Associated bookings cleared.\n";
                allBooths.erase(it);
                std::cout << "Booth deleted successfully from the system.\n";
            }
            system("pause");
            break;
        }
        case 0: return;
        default: std::cout << "Invalid choice. Please try again.\n"; system("pause"); break;
        }
    }
}

static void manageBookingsForFloor(const std::string& mallName, int floorLevel, const std::vector<std::unique_ptr<Booth>>& allBooths, const std::vector<std::unique_ptr<Vendor>>& allVendors, std::vector<std::unique_ptr<Booking>>& allBookings, std::vector<std::unique_ptr<Transaction>>& transactions) {
    int choice = -1;
    while (choice != 0) {
        system("cls");
        std::cout << "\n--- Booking Management for " << mallName << " - Floor " << floorLevel << " ---\n";
        std::cout << "1. View All Bookings\n";
        std::cout << "2. Create New Booking\n";
        std::cout << "3. Cancel a Booking\n";
        std::cout << "0. Return to Floor Menu\n";
        std::cout << "---------------------------------------------------\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1;
        }
        switch (choice) {
        case 1: {
            system("cls");
            std::cout << "--- All Confirmed Bookings for " << mallName << " - Floor " << floorLevel << " ---\n\n";
            bool found = false;
            for (const auto& booking : allBookings) {
                const Booth* b = GenericUtils::findById(allBooths, booking->boothId);
                if (b && b->mallName == mallName && b->floorLevel == floorLevel && booking->status == "Confirmed") {
                    const Vendor* vendor = GenericUtils::findById(allVendors, booking->vendorId);
                    std::string vendorName = vendor ? vendor->name : "Unknown Vendor";
                    std::cout << "============================================\n";
                    std::cout << " Booking ID: " << booking->bookingId << "\n";
                    std::cout << "--------------------------------------------\n";
                    std::cout << "   Booth ID: " << booking->boothId << "\n";
                    std::cout << "     Vendor: " << vendorName << " (ID: " << booking->vendorId << ")\n";
                    std::cout << "  Date Span: " << booking->startDate << " to " << booking->endDate << "\n";
                    std::cout << "      Terms: " << (booking->contractTerms.empty() ? "N/A" : booking->contractTerms) << "\n";
                    std::cout << " Regulations: " << (booking->regulations.empty() ? "N/A" : booking->regulations) << "\n";
                    std::cout << "============================================\n\n";
                    found = true;
                }
            }
            if (!found) { std::cout << "No bookings found for this floor.\n"; }
            system("pause");
            break;
        }
        case 2: {
            system("cls");
            std::cout << "--- Create New Booking ---\n";
            std::string boothId;
            std::cout << "Enter Booth ID to book: ";
            std::cin >> boothId;
            Booth* b = GenericUtils::findById(const_cast<std::vector<std::unique_ptr<Booth>>&>(allBooths), boothId);
            if (b == nullptr || b->mallName != mallName || b->floorLevel != floorLevel) {
                std::cout << "Error: Booth ID not found on this floor.\n"; system("pause"); break;
            }
            std::cout << "Enter Vendor ID for this booking: ";
            int vendorId;
            std::cin >> vendorId;
            if (GenericUtils::findById(allVendors, vendorId) == nullptr) {
                std::cout << "Error: Vendor ID not found.\n"; system("pause"); break;
            }
            // ============================ FIX START ============================
            std::string startDate, endDate;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            while (true) {
                std::cout << "Enter Start Date (YYYY-MM-DD): ";
                getline(std::cin, startDate);
                if (isValidDate(startDate)) break;
                system("pause");
            }

            while (true) {
                std::cout << "Enter End Date (YYYY-MM-DD): ";
                getline(std::cin, endDate);
                if (isValidDate(endDate) && endDate >= startDate) break;
                else if (endDate < startDate) std::cout << "Error: End date cannot be before start date.\n";
                system("pause");
            }
            // ============================= FIX END =============================

            int newId = 1;
            if (!allBookings.empty()) { newId = (*std::max_element(allBookings.begin(), allBookings.end(), [](const auto& a, const auto& b) { return a->bookingId < b->bookingId; }))->bookingId + 1; }
            allBookings.push_back(std::make_unique<Booking>(newId, boothId, vendorId, startDate, endDate));
            std::cout << "Booking created successfully with ID " << newId << "!\n";

            int newTransId = transactions.empty() ? 1 : transactions.back()->transactionId + 1;
            std::string desc = "Booth Rental Fee " + b->boothId;
            transactions.push_back(std::make_unique<Transaction>(newTransId, vendorId, b->price, desc));
            std::cout << "Transaction for $" << std::fixed << std::setprecision(2) << b->price << " created.\n";
            system("pause");
            break;
        }
        case 3: {
            system("cls");
            std::cout << "--- Cancel Booking ---\n";
            std::cout << "Enter Booking ID to cancel: ";
            int bookingId;
            std::cin >> bookingId;
            auto it = std::find_if(allBookings.begin(), allBookings.end(), [bookingId](const std::unique_ptr<Booking>& b) { return b->bookingId == bookingId; });
            if (it == allBookings.end()) {
                std::cout << "Error: Booking ID not found.\n";
            }
            else {
                const Booking* bookingToCancel = it->get();
                Booth* relatedBooth = GenericUtils::findById(const_cast<std::vector<std::unique_ptr<Booth>>&>(allBooths), bookingToCancel->boothId);
                if (relatedBooth && relatedBooth->assignedVendorId == bookingToCancel->vendorId) {
                    std::cout << "Warning: This booking is for a currently checked-in vendor. Checking them out.\n";
                    relatedBooth->assignedVendorId = 0;
                }
                allBookings.erase(it);
                std::cout << "Booking cancelled successfully.\n";
            }
            system("pause");
            break;
        }
        case 0: return;
        default: std::cout << "Invalid choice. Please try again.\n"; system("pause"); break;
        }
    }
}

static void manageLogisticsForBooth(Booth* booth, std::vector<std::unique_ptr<Logistics>>& allLogistics) {
    if (!booth) return;
    if (booth->assignedVendorId == 0) {
        std::cout << "Error: A vendor must be assigned to this booth before managing logistics.\n";
        system("pause"); return;
    }
    int choice = -1;
    while (choice != 0) {
        system("cls");
        std::cout << "--- Logistics for Booth " << booth->boothId << " ---\n";
        std::cout << "Assigned to Vendor ID: " << booth->assignedVendorId << "\n\n";
        std::cout << "1. Request New Item\n";
        std::cout << "2. View Current Requests\n";
        std::cout << "3. Update Request Status\n";
        std::cout << "4. Cancel a Request\n";
        std::cout << "0. Return to Booth Menu\n";
        std::cout << "--------------------------------\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1;
        }
        switch (choice) {
        case 1: {
            system("cls");
            std::cout << "--- Request New Logistics Item ---\n";
            std::cout << "Available Items:\n  1. Table   ($50.00)\n  2. Chair   ($20.00)\n  3. Power Outlet ($100.00)\n";
            std::cout << "Enter item choice: ";
            int itemChoice;
            std::cin >> itemChoice;
            std::string itemName;
            double unitCost;
            switch (itemChoice) {
            case 1: itemName = "Table"; unitCost = 50.0; break;
            case 2: itemName = "Chair"; unitCost = 20.0; break;
            case 3: itemName = "Power Outlet"; unitCost = 100.0; break;
            default: std::cout << "Invalid item choice.\n"; system("pause"); continue;
            }
            std::cout << "Enter quantity: ";
            int qty;
            std::cin >> qty;
            if (std::cin.fail() || qty <= 0) {
                std::cout << "Invalid quantity.\n"; system("pause"); continue;
            }
            int newId = 1;
            if (!allLogistics.empty()) {
                newId = (*std::max_element(allLogistics.begin(), allLogistics.end(),
                    [](const auto& a, const auto& b) { return a->logisticsId < b->logisticsId; }))->logisticsId + 1;
            }
            allLogistics.push_back(std::make_unique<Logistics>(newId, booth->boothId, itemName, qty, unitCost));
            std::cout << "Item requested successfully! Logistics ID: " << newId << "\n";
            system("pause");
            break;
        }
        case 2: {
            system("cls");
            std::cout << "--- Current Logistics for Booth " << booth->boothId << " ---\n";
            std::vector<const Logistics*> boothLogistics;
            for (const auto& log : allLogistics) {
                if (log->boothId == booth->boothId) { boothLogistics.push_back(log.get()); }
            }
            if (boothLogistics.empty()) {
                std::cout << "No logistics requests found for this booth.\n";
            }
            else {
                const int id_w = 6, item_w = 15, qty_w = 5, cost_w = 12, status_w = 12;
                std::string border = "+" + std::string(id_w, '-') + "+" + std::string(item_w, '-') + "+" + std::string(qty_w, '-') + "+" + std::string(cost_w, '-') + "+" + std::string(status_w, '-') + "+\n";
                std::cout << border;
                std::cout << "| " << std::left << std::setw(id_w - 2) << "ID" << " | "
                    << std::setw(item_w - 2) << "Item" << " | "
                    << std::setw(qty_w - 2) << "Qty" << " | "
                    << std::setw(cost_w - 2) << "Total Cost" << " | "
                    << std::setw(status_w - 2) << "Status" << " |\n";
                std::cout << border;
                for (const auto* log : boothLogistics) {
                    std::cout << "| " << std::left << std::setw(id_w - 2) << log->logisticsId << " | "
                        << std::setw(item_w - 2) << log->itemName << " | "
                        << std::right << std::setw(qty_w - 2) << log->quantity << " | "
                        << "$" << std::setw(cost_w - 3) << std::fixed << std::setprecision(2) << log->getTotalCost() << " | "
                        << std::left << std::setw(status_w - 2) << log->status << " |\n";
                }
                std::cout << border;
            }
            system("pause");
            break;
        }
        case 3: {
            system("cls");
            std::cout << "--- Update Logistics Status ---\n";
            std::cout << "Enter Logistics ID to update: ";
            int logId;
            std::cin >> logId;
            Logistics* targetLogistics = nullptr;
            for (auto& log : allLogistics) {
                if (log->logisticsId == logId) { targetLogistics = log.get(); break; }
            }
            if (targetLogistics == nullptr || targetLogistics->boothId != booth->boothId) {
                std::cout << "Error: Logistics ID not found for this booth.\n";
            }
            else {
                std::cout << "Enter new status (e.g., Delivered, Cancelled): ";
                std::cin.ignore();
                getline(std::cin, targetLogistics->status);
                std::cout << "Status updated successfully!\n";
            }
            system("pause");
            break;
        }
        case 4: {
            system("cls");
            std::cout << "--- Cancel Logistics Request ---\n";
            std::cout << "Enter Logistics ID to cancel: ";
            int logId;
            std::cin >> logId;
            auto it = std::find_if(allLogistics.begin(), allLogistics.end(), [logId, booth](const std::unique_ptr<Logistics>& log) {
                return log->logisticsId == logId && log->boothId == booth->boothId;
                });
            if (it == allLogistics.end()) {
                std::cout << "Error: Logistics ID not found for this booth.\n";
            }
            else {
                allLogistics.erase(it);
                std::cout << "Request cancelled and deleted successfully.\n";
            }
            system("pause");
            break;
        }
        case 0: break;
        default: std::cout << "Invalid choice. Please try again.\n"; system("pause"); break;
        }
    }
}

namespace BoothManagement {
    void manageBooths(std::vector<std::unique_ptr<Booth>>& allBooths, const std::vector<std::unique_ptr<Vendor>>& allVendors,
        std::vector<std::unique_ptr<Logistics>>& allLogistics, std::vector<std::unique_ptr<Booking>>& allBookings,
        std::vector<std::unique_ptr<Transaction>>& transactions) {
        while (true) {
            system("cls");
            std::cout << "\n--- Select a Location to Manage ---\n";
            std::cout << "1. Mid-Valley (ComicFiesta)\n";
            std::cout << "2. Pavilion (Tech Expos)\n";
            std::cout << "3. TRX (Car Expos)\n";
            std::cout << "0. Back to Main Menu\n";
            std::cout << "-----------------------------------\n";
            std::cout << "Enter choice: ";
            int mallChoice;
            std::cin >> mallChoice;
            if (std::cin.fail()) {
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); mallChoice = -1;
            }
            std::string selectedMall;
            switch (mallChoice) {
            case 1: selectedMall = "Mid-Valley"; break;
            case 2: selectedMall = "Pavilion"; break;
            case 3: selectedMall = "TRX"; break;
            case 0: return;
            default: std::cout << "Invalid choice.\n"; system("pause"); continue;
            }
            system("cls");
            std::cout << "\n--- Select a Floor for " << selectedMall << " ---\n";
            std::cout << "1. Level 1\n";
            std::cout << "2. Level 2\n";
            std::cout << "3. Level 3\n";
            std::cout << "0. Back to Location Selection\n";
            std::cout << "Enter choice: ";
            int floorChoice;
            std::cin >> floorChoice;
            if (std::cin.fail()) {
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); floorChoice = -1;
            }
            if (floorChoice >= 1 && floorChoice <= 3) {
                manageSingleFloorBooths(allBooths, allVendors, allLogistics, allBookings, transactions, selectedMall, floorChoice);
            }
            else if (floorChoice == 0) {
                continue;
            }
            else {
                std::cout << "Invalid floor.\n";
                system("pause");
            }
        }
    }
}