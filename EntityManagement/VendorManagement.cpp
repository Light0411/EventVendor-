#define NOMINMAX
#include "VendorManagement.h"
#include "ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Vendor.h"
#include "../Entity/Booth.h"
#include "../Entity/Booking.h"
#include "../Entity/Transaction.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <map>
#include <regex>

const std::map<std::string, std::string> mallToEventName = {
    {"Mid-Valley", "ComicFiesta"},
    {"Pavilion", "Tech Expos"},
    {"TRX", "Car Expos"}
};

static bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    return std::regex_match(email, pattern);
}

static int getVendorMenuChoice() {
    system("cls");
    std::cout << "\n--- Manage Vendors ---\n";
    std::cout << "1. Add New Vendor\n";
    std::cout << "2. View All Vendors\n";
    std::cout << "3. Update Vendor\n";
    std::cout << "4. Delete Vendor\n";
    std::cout << "5. Create Contract/Booking for Vendor\n";
    std::cout << "0. Return to Main Menu\n";
    std::cout << "-------------------------------------\n";
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); return -1;
    }
    return choice;
}

static void createContractForVendor(
    const std::vector<std::unique_ptr<Vendor>>& vendors,
    const std::vector<std::unique_ptr<Booth>>& booths,
    std::vector<std::unique_ptr<Booking>>& bookings,
    std::vector<std::unique_ptr<Transaction>>& transactions
);

namespace VendorManagement {
    void manageVendors(
        std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Booth>>& booths,
        std::vector<std::unique_ptr<Booking>>& bookings,
        std::vector<std::unique_ptr<Transaction>>& transactions
    ) {
        int choice = -1;
        while (choice != 0) {
            choice = getVendorMenuChoice();
            switch (choice) {
            case 1: { // Add New Vendor
                system("cls");
                std::cout << "--- Add New Vendor ---\n";
                int id;
                std::string name, email;
                std::cout << "Enter Vendor ID: ";
                std::cin >> id;
                if (std::cin.fail() || GenericUtils::findById(vendors, id) != nullptr) {
                    std::cout << "Error: Invalid or duplicate ID.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause"); break;
                }
                std::cout << "Enter Name (max 14 characters): ";
                std::cin.ignore(); getline(std::cin, name);
                if (name.length() >= 15) {
                    std::cout << "Error: Vendor name is too long.\n";
                    system("pause"); break;
                }
                std::cout << "Enter Email: ";
                getline(std::cin, email);
                if (!isValidEmail(email)) {
                    std::cout << "Error: Invalid email format.\n";
                    system("pause"); break;
                }
                auto newVendor = std::make_unique<Vendor>(id, name, email);

                char addEvent = 'y';
                do {
                    system("cls");
                    std::cout << "\n--- Register Vendor for an Event Location ---\n";
                    std::cout << "1. Mid-Valley (ComicFiesta)\n";
                    std::cout << "2. Pavilion (Tech Expos)\n";
                    std::cout << "3. TRX (Car Expos)\n";
                    std::cout << "0. Finished\n";
                    std::cout << "---------------------------------------------\n";
                    std::cout << "Enter choice: ";
                    int eventChoice;
                    std::cin >> eventChoice;
                    if (eventChoice == 0) { break; }
                    std::string selectedMall;
                    switch (eventChoice) {
                    case 1: selectedMall = "Mid-Valley"; break;
                    case 2: selectedMall = "Pavilion"; break;
                    case 3: selectedMall = "TRX"; break;
                    default: std::cout << "Invalid choice.\n"; system("pause"); continue;
                    }
                    if (!selectedMall.empty()) {
                        if (std::find(newVendor->registeredEvents.begin(), newVendor->registeredEvents.end(), selectedMall) == newVendor->registeredEvents.end()) {
                            newVendor->registeredEvents.push_back(selectedMall);
                            std::cout << "Vendor registered for event at " << selectedMall << ".\n";
                        }
                        else {
                            std::cout << "Vendor is already registered for this event location.\n";
                        }
                    }
                    std::cout << "Register for another event location? (y/n): ";
                    std::cin >> addEvent;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                } while (addEvent == 'y' || addEvent == 'Y');
                vendors.push_back(std::move(newVendor));
                std::cout << "Vendor added successfully!\n";
                system("pause");
                break;
            }
            case 2: { // View All Vendors
                system("cls");
                std::cout << "--- All Registered Vendors ---\n\n";
                if (vendors.empty()) {
                    std::cout << "No vendors found in the system.\n";
                }
                else {
                    const int idWidth = 8, nameWidth = 18, emailWidth = 28, eventWidth = 26, bookedWidth = 15, contractWidth = 85;
                    std::string border = "+" + std::string(idWidth, '-') + "+" + std::string(nameWidth, '-') + "+" + std::string(emailWidth, '-') + "+" + std::string(eventWidth, '-') + "+" + std::string(bookedWidth, '-') + "+" + std::string(contractWidth, '-') + "+\n";
                    std::cout << border;
                    std::cout << "| " << std::left << std::setw(idWidth - 2) << "ID" << " | "
                        << std::setw(nameWidth - 2) << "Name" << " | "
                        << std::setw(emailWidth - 2) << "Email" << " | "
                        << std::setw(eventWidth - 2) << "Registered Events" << " | "
                        << std::setw(bookedWidth - 2) << "Booked Booths" << " | "
                        << std::setw(contractWidth - 2) << "Contract Details" << " |\n";
                    std::cout << border;

                    for (const auto& vendor : vendors) {
                        std::string eventsDisplayStr, bookedBoothsStr, contractStr;

                        if (!vendor->registeredEvents.empty()) {
                            for (size_t i = 0; i < vendor->registeredEvents.size(); ++i) {
                                std::string mall = vendor->registeredEvents[i];
                                std::string eventName = mallToEventName.count(mall) ? mallToEventName.at(mall) : mall;
                                eventsDisplayStr += eventName + (i < vendor->registeredEvents.size() - 1 ? ", " : "");
                            }
                        }
                        else { eventsDisplayStr = "None"; }
                        if (eventsDisplayStr.length() > eventWidth - 2) eventsDisplayStr = eventsDisplayStr.substr(0, eventWidth - 3) + ".";

                        const Booking* firstBooking = nullptr;
                        for (const auto& booking : bookings) {
                            if (booking->vendorId == vendor->vendorId) {
                                if (!firstBooking) firstBooking = booking.get();
                                if (!bookedBoothsStr.empty()) bookedBoothsStr += ",";
                                bookedBoothsStr += booking->boothId;
                            }
                        }
                        if (bookedBoothsStr.empty()) bookedBoothsStr = "N/A";
                        if (bookedBoothsStr.length() > bookedWidth - 2) bookedBoothsStr = bookedBoothsStr.substr(0, bookedWidth - 3) + ".";

                        if (firstBooking) {
                            std::stringstream contractStream;
                            std::string terms = firstBooking->contractTerms.empty() ? "N/A" : firstBooking->contractTerms;
                            std::string regs = firstBooking->regulations.empty() ? "N/A" : firstBooking->regulations;

                            const int termsContentWidth = 35;

                            if (terms.length() > termsContentWidth) {
                                terms = terms.substr(0, termsContentWidth - 3) + "...";
                            }

                            contractStream << "T: " << std::left << std::setw(termsContentWidth) << terms << " | R: " << regs;

                            contractStr = contractStream.str();

                            if (contractStr.length() > contractWidth - 2) {
                                contractStr = contractStr.substr(0, contractWidth - 5) + "...";
                            }
                        }
                        else {
                            contractStr = "No active contracts";
                        }

                        std::cout << "| " << std::left << std::setw(idWidth - 2) << vendor->vendorId << " | "
                            << std::setw(nameWidth - 2) << vendor->name << " | "
                            << std::setw(emailWidth - 2) << vendor->email << " | "
                            << std::setw(eventWidth - 2) << eventsDisplayStr << " | "
                            << std::setw(bookedWidth - 2) << bookedBoothsStr << " | "
                            << std::setw(contractWidth - 2) << contractStr << " |\n";
                    }
                    std::cout << border;
                }
                system("pause");
                break;
            }
            case 3: { // Update Vendor
                system("cls");
                std::cout << "--- Update Vendor ---\n";
                int id;
                std::cout << "Enter ID of vendor to update: ";
                std::cin >> id;
                Vendor* vendor = GenericUtils::findById(vendors, id);
                if (vendor == nullptr) {
                    std::cout << "Error: Vendor not found.\n";
                }
                else {
                    std::string newName, newEmail;
                    std::cout << "Enter new Name (current: " << vendor->name << "): ";
                    std::cin.ignore(); getline(std::cin, newName);
                    vendor->name = newName;
                    std::cout << "Enter new Email (current: " << vendor->email << "): ";
                    getline(std::cin, newEmail);
                    if (!isValidEmail(newEmail)) {
                        std::cout << "Error: Invalid email format. Update cancelled.\n";
                    }
                    else {
                        vendor->email = newEmail;
                        std::cout << "Vendor updated successfully!\n";
                    }
                }
                system("pause");
                break;
            }
            case 4: { // Delete Vendor
                system("cls");
                std::cout << "--- Delete Vendor ---\n";
                int id;
                std::cout << "Enter ID of vendor to delete: ";
                std::cin >> id;

                auto it = std::find_if(vendors.begin(), vendors.end(),
                    [id](const std::unique_ptr<Vendor>& v) { return v->vendorId == id; });

                if (it == vendors.end()) {
                    std::cout << "Error: Vendor not found.\n";
                }
                else {
                    const auto bk_before = bookings.size();
                    bookings.erase(std::remove_if(bookings.begin(), bookings.end(),
                        [id](const std::unique_ptr<Booking>& b) { return b->vendorId == id; }),
                        bookings.end());
                    const auto bk_removed = bk_before - bookings.size();
                    const auto tr_before = transactions.size();
                    transactions.erase(std::remove_if(transactions.begin(), transactions.end(),
                        [id](const std::unique_ptr<Transaction>& t) { return t->vendorId == id; }),
                        transactions.end());
                    const auto tr_removed = tr_before - transactions.size();
                    vendors.erase(it);

                    std::cout << "Vendor deleted. Removed " << bk_removed
                        << " booking(s) and " << tr_removed << " transaction(s).\n";
                }
                system("pause");
                break;
            }
            case 5: {
                createContractForVendor(vendors, booths, bookings, transactions);
                break;
            }
            case 0: return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                system("pause");
                break;
            }
        }
    }
}


static void createContractForVendor(
    const std::vector<std::unique_ptr<Vendor>>& vendors,
    const std::vector<std::unique_ptr<Booth>>& booths,
    std::vector<std::unique_ptr<Booking>>& bookings,
    std::vector<std::unique_ptr<Transaction>>& transactions)
{
    system("cls");
    std::cout << "--- Create New Contract/Booking ---\n";
    int vendorId;
    std::cout << "Enter Vendor ID for this contract: ";
    std::cin >> vendorId;
    if (std::cin.fail() || GenericUtils::findById(vendors, vendorId) == nullptr) {
        std::cout << "Error: Vendor ID not found.\n";
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        system("pause"); return;
    }
    std::string boothId;
    std::cout << "Enter Booth ID to book (e.g., M101): ";
    std::cin >> boothId;
    Booth* b = GenericUtils::findById(const_cast<std::vector<std::unique_ptr<Booth>>&>(booths), boothId);
    if (b == nullptr) {
        std::cout << "Error: Booth ID not found.\n";
        system("pause"); return;
    }

    // ============================ FIX START ============================
    std::string startDate, endDate, terms, regs;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer before getting dates

    // Validate Start Date
    while (true) {
        std::cout << "Enter Start Date (YYYY-MM-DD): ";
        getline(std::cin, startDate);
        if (isValidDate(startDate)) {
            break; // Exit loop if date is valid
        }
        // isValidDate function will print the specific error
        system("pause");
    }

    // Validate End Date
    while (true) {
        std::cout << "Enter End Date (YYYY-MM-DD): ";
        getline(std::cin, endDate);
        if (isValidDate(endDate)) {
            if (endDate >= startDate) {
                break; // Exit loop if date is valid and not before start date
            }
            else {
                std::cout << "Error: End date cannot be before the start date.\n";
            }
        }
        system("pause");
    }

    std::cout << "Enter Payment Terms: ";
    getline(std::cin, terms);
    std::cout << "Enter Regulations Agreement: ";
    getline(std::cin, regs);

    int newId = 1;
    if (!bookings.empty()) {
        newId = (*std::max_element(bookings.begin(), bookings.end(),
            [](const auto& a, const auto& b) { return a->bookingId < b->bookingId; }))->bookingId + 1;
    }
    auto newBooking = std::make_unique<Booking>(newId, boothId, vendorId, startDate, endDate);
    newBooking->contractTerms = terms;
    newBooking->regulations = regs;
    bookings.push_back(std::move(newBooking));
    std::cout << "\nBooking created successfully with Booking ID " << newId << "!\n";

    int newTransId = transactions.empty() ? 1 : transactions.back()->transactionId + 1;
    std::string desc = "Booth Rental Fee " + b->boothId;
    transactions.push_back(std::make_unique<Transaction>(newTransId, vendorId, b->price, desc));
    std::cout << "Transaction for $" << std::fixed << std::setprecision(2) << b->price << " created.\n";
    system("pause");
}