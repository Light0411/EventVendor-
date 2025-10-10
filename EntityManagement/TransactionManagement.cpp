#define NOMINMAX
#include "TransactionManagement.h"
#include "ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Transaction.h"
#include "../Entity/Vendor.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

// --- HELPER transaction table display ---
static void displayTransactionsTable(const std::vector<Transaction*>& transactionList, const std::vector<std::unique_ptr<Vendor>>& allVendors, const std::string& title) {
    system("cls");
    std::cout << "--- " << title << " ---\n\n";
    if (transactionList.empty()) {
        std::cout << "No transactions to display.\n";
    }
    else {
        const int id_w = 16, vid_w = 11, vname_w = 18, desc_w = 30, amount_w = 15;
        std::stringstream border;
        border << "+" << std::string(id_w + 2, '-') << "+" << std::string(vid_w + 2, '-') << "+" << std::string(vname_w + 2, '-') << "+" << std::string(desc_w + 2, '-') << "+" << std::string(amount_w + 2, '-') << "+";
        std::cout << border.str() << "\n";
        std::cout << "| " << std::left << std::setw(id_w) << "Transaction ID" << " "
            << "| " << std::setw(vid_w) << "Vendor ID" << " "
            << "| " << std::setw(vname_w) << "Vendor Name" << " "
            << "| " << std::setw(desc_w) << "Description" << " "
            << "| " << std::setw(amount_w) << "Amount" << " |\n";
        std::cout << border.str() << "\n";
        for (const auto* trans : transactionList) {
            const Vendor* vendor = GenericUtils::findById(allVendors, trans->vendorId);
            std::string vendorName = vendor ? vendor->name : (trans->vendorId == 0 ? "N/A (Event)" : "Unknown");
            if (vendorName.length() > vname_w) vendorName = vendorName.substr(0, vname_w - 1) + ".";
            std::string transDesc = trans->description;
            if (transDesc.length() > desc_w) transDesc = transDesc.substr(0, desc_w - 1) + ".";
            std::cout << "| " << std::left << std::setw(id_w) << trans->transactionId << " "
                << "| " << std::setw(vid_w) << trans->vendorId << " "
                << "| " << std::setw(vname_w) << vendorName << " "
                << "| " << std::setw(desc_w) << transDesc << " "
                << "| $" << std::right << std::setw(amount_w - 1) << std::fixed << std::setprecision(2) << trans->amount << " |\n";
        }
        std::cout << border.str() << "\n";
    }
}

// --- helper calc and display vendor's total ---
static void calculateAndDisplayVendorTotal(const std::vector<std::unique_ptr<Transaction>>& allTransactions, const std::vector<std::unique_ptr<Vendor>>& allVendors) {
    std::cout << "Enter Vendor ID or full Vendor Name to calculate total: ";
    std::string input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(std::cin, input);
    const Vendor* foundVendor = nullptr;
    try {
        int vendorId = std::stoi(input);
        foundVendor = GenericUtils::findById(allVendors, vendorId);
    }
    catch (const std::exception&) {
        for (const auto& v : allVendors) { if (v->name == input) { foundVendor = v.get(); break; } }
    }
    if (!foundVendor) {
        std::cout << "Error: Vendor not found.\n";
    }
    else {
        double totalAmount = 0.0;
        for (const auto& t : allTransactions) { if (t->vendorId == foundVendor->vendorId) { totalAmount += t->amount; } }
        std::cout << "\nTotal transaction amount for vendor '" << foundVendor->name << "': $"
            << std::fixed << std::setprecision(2) << totalAmount << "\n";
    }
    system("pause");
}

namespace TransactionManagement {

    void manageTransactions(std::vector<std::unique_ptr<Transaction>>& transactions, const std::vector<std::unique_ptr<Vendor>>& allVendors) {
        int choice = -1;
        while (choice != 0) {
            system("cls");
            std::cout << "\n--- Manage Transactions ---\n";
            std::cout << "1. Add New Transaction\n";
            std::cout << "2. View All Transactions\n";
            std::cout << "3. Delete Transaction\n";
            std::cout << "0. Return to Main Menu\n";
            std::cout << "-----------------------------\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); choice = -1; }

            switch (choice) {
            case 1: { // Add
                system("cls");
                std::cout << "--- Add New Manual Transaction ---\n";
                int vendorId;
                double amount;
                std::string description;

                std::cout << "Enter Vendor ID to associate with this transaction (or 0 for event-related): ";
                std::cin >> vendorId;
                if (std::cin.fail()) {
                    std::cout << "Error: Invalid Vendor ID input.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                // Check if vendor exists, unless it's the special '0' ID
                if (vendorId != 0 && GenericUtils::findById(allVendors, vendorId) == nullptr) {
                    std::cout << "Error: Vendor with ID " << vendorId << " not found.\n";
                    system("pause");
                    break;
                }

                // Get transaction amount
                std::cout << "Enter Amount (use a negative number for refunds or expenses): ";
                std::cin >> amount;
                if (std::cin.fail()) {
                    std::cout << "Error: Invalid amount input.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                // Get description
                std::cout << "Enter Description: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                getline(std::cin, description);

                // Generate new transaction ID
                int newTransId = 1;
                if (!transactions.empty()) {
                    newTransId = (*std::max_element(transactions.begin(), transactions.end(),
                        [](const auto& a, const auto& b) { return a->transactionId < b->transactionId; }))->transactionId + 1;
                }

                // Create and add the new transaction
                transactions.push_back(std::make_unique<Transaction>(newTransId, vendorId, amount, description));
                std::cout << "\nTransaction with ID " << newTransId << " added successfully!\n";
                system("pause");
                break;
            }
            case 2: { // View
                int viewChoice = -1;
                std::vector<Transaction*> transactionPtrs;
                for (const auto& t : transactions) { transactionPtrs.push_back(t.get()); }
                std::string currentTitle = "All Transactions";
                bool resetView = false;
                auto vendorNameMap = GenericUtils::createIdToNameMap(allVendors);
                while (viewChoice != 0) {
                    if (resetView) {
                        transactionPtrs.clear();
                        for (const auto& t : transactions) { transactionPtrs.push_back(t.get()); }
                        currentTitle = "All Transactions";
                        resetView = false;
                    }
                    displayTransactionsTable(transactionPtrs, allVendors, currentTitle);
                    std::cout << "\n--- Options ---\n";
                    std::cout << "1. Sort by Vendor ID\n";
                    std::cout << "2. Sort by Vendor Name (A-Z)\n";
                    std::cout << "3. Sort by Amount (Low to High)\n";
                    std::cout << "4. Sort by Amount (High to Low)\n";
                    std::cout << "5. Find Transactions by Vendor\n";
                    std::cout << "6. Calculate Vendor Total\n";
                    std::cout << "7. Reset View\n";
                    std::cout << "0. Back to Transaction Menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> viewChoice;
                    if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); viewChoice = -1; }
                    switch (viewChoice) {
                    case 1:
                        std::sort(transactionPtrs.begin(), transactionPtrs.end(), [](const auto* a, const auto* b) { return a->vendorId < b->vendorId; });
                        currentTitle = "Transactions Sorted by Vendor ID";
                        break;
                    case 2:
                        std::sort(transactionPtrs.begin(), transactionPtrs.end(),
                            [&vendorNameMap](const Transaction* a, const Transaction* b) {
                                std::string nameA = vendorNameMap.count(a->vendorId) ? vendorNameMap.at(a->vendorId) : "";
                                std::string nameB = vendorNameMap.count(b->vendorId) ? vendorNameMap.at(b->vendorId) : "";
                                return nameA < nameB;
                            });
                        currentTitle = "Transactions Sorted by Vendor Name";
                        break;
                    case 3:
                        std::sort(transactionPtrs.begin(), transactionPtrs.end(), [](const auto* a, const auto* b) { return a->amount < b->amount; });
                        currentTitle = "Transactions Sorted by Amount (Low-High)";
                        break;
                    case 4:
                        std::sort(transactionPtrs.begin(), transactionPtrs.end(), [](const auto* a, const auto* b) { return a->amount > b->amount; });
                        currentTitle = "Transactions Sorted by Amount (High-Low)";
                        break;
                    case 5: {
                        std::cout << "Enter Vendor ID or full Vendor Name to find: ";
                        std::string input;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        getline(std::cin, input);
                        const Vendor* foundVendor = nullptr;
                        try {
                            int vendorId = std::stoi(input);
                            foundVendor = GenericUtils::findById(allVendors, vendorId);
                        }
                        catch (const std::exception&) {
                            for (const auto& v : allVendors) { if (v->name == input) { foundVendor = v.get(); break; } }
                        }
                        if (!foundVendor) {
                            std::cout << "Error: Vendor not found.\n";
                            system("pause");
                        }
                        else {
                            transactionPtrs.erase(std::remove_if(transactionPtrs.begin(), transactionPtrs.end(),
                                [foundVendor](const Transaction* t) { return t->vendorId != foundVendor->vendorId; }), transactionPtrs.end());
                            currentTitle = "Transactions for Vendor: " + foundVendor->name;
                        }
                        break;
                    }
                    case 6:
                        calculateAndDisplayVendorTotal(transactions, allVendors);
                        break;
                    case 7:
                        resetView = true;
                        break;
                    case 0: break;
                    default:
                        std::cout << "Invalid view choice.\n";
                        system("pause");
                        break;
                    }
                }
                break;
            }
            case 3: { // Delete Transaction
                system("cls");
                std::cout << "--- Delete Transaction ---\n";
                int id;
                std::cout << "Enter ID of transaction to delete: ";
                std::cin >> id;

                auto it = std::find_if(transactions.begin(), transactions.end(), [id](const std::unique_ptr<Transaction>& t) {
                    return t->transactionId == id;
                    });

                if (it == transactions.end()) {
                    std::cout << "Error: Transaction not found.\n";
                }
                else {
                    transactions.erase(it);
                    std::cout << "Transaction deleted successfully.\n";
                }
                system("pause");
                break;
            }
            case 0:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                system("pause");
                break;
            }
        }
    }
}