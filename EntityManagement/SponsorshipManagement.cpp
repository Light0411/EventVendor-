#define NOMINMAX
#include "SponsorshipManagement.h"
#include "../Entity/Sponsorship.h"
#include "../Entity/Transaction.h"
#include "ModuleHelpers.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <memory>
#include <algorithm>

namespace SponsorshipManagement {
    void manageSponsorships(std::vector<std::unique_ptr<Sponsorship>>& sponsorships, std::vector<std::unique_ptr<Transaction>>& transactions) {
        int choice = -1;
        while (choice != 0) {
            choice = getSubMenuChoice("Sponsorships");
            switch (choice) {
            case 1: { // Add
                system("cls");
                std::cout << "--- Add New Sponsorship ---\n";
                int id;
                std::string name, desc;
                double amount;
                std::cout << "Enter Sponsor ID: ";
                std::cin >> id;
                if (std::cin.fail() || std::any_of(sponsorships.begin(), sponsorships.end(), [id](const auto& s) { return s->sponsorId == id; })) {
                    std::cout << "Error: Invalid or duplicate ID.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Enter Sponsor Name: ";
                getline(std::cin, name);
                std::cout << "Enter Sponsorship Amount: ";
                std::cin >> amount;
                if (std::cin.fail() || amount <= 0) {
                    std::cout << "Error: Invalid amount.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Enter Description (e.g., Gold Tier): ";
                getline(std::cin, desc);

                sponsorships.push_back(std::make_unique<Sponsorship>(id, name, amount, desc));

                int newTransId = transactions.empty() ? 1 : transactions.back()->transactionId + 1;
                transactions.push_back(std::make_unique<Transaction>(newTransId, 0, amount, "Sponsorship: " + name));

                std::cout << "Sponsorship added successfully!\n";
                system("pause");
                break;
            }
            case 2: { // View
                system("cls");
                std::cout << "--- All Sponsorships ---\n\n";
                if (sponsorships.empty()) {
                    std::cout << "No sponsorships found.\n";
                }
                else {
                    const int id_w = 8, name_w = 25, amount_w = 15, desc_w = 30;
                    std::string border = "+" + std::string(id_w, '-') + "+" + std::string(name_w, '-') + "+" + std::string(amount_w, '-') + "+" + std::string(desc_w, '-') + "+\n";
                    std::cout << border;
                    std::cout << "| " << std::left << std::setw(id_w - 2) << "ID" << " | " << std::setw(name_w - 2) << "Sponsor Name" << " | " << std::setw(amount_w - 2) << "Amount" << " | " << std::setw(desc_w - 2) << "Description" << " |\n";
                    std::cout << border;
                    for (const auto& s : sponsorships) {
                        std::cout << "| " << std::left << std::setw(id_w - 2) << s->sponsorId << " | " << std::setw(name_w - 2) << s->name << " | " << "$" << std::right << std::setw(amount_w - 3) << std::fixed << std::setprecision(2) << s->amount << " | " << std::left << std::setw(desc_w - 2) << s->description << " |\n";
                    }
                    std::cout << border;
                }
                system("pause");
                break;
            }
            case 3: { // Update
                system("cls");
                std::cout << "--- Update Sponsorship ---\n";
                int id;
                std::cout << "Enter ID of sponsor to update: ";
                std::cin >> id;

                Sponsorship* sponsorToUpdate = nullptr;
                for (const auto& s : sponsorships) {
                    if (s->sponsorId == id) {
                        sponsorToUpdate = s.get();
                        break;
                    }
                }

                if (sponsorToUpdate == nullptr) {
                    std::cout << "Error: Sponsorship ID not found.\n";
                }
                else {
                    std::string oldName = sponsorToUpdate->name;
                    std::string newName, newDesc;
                    double newAmount;

                    std::cout << "Enter new Name (current: " << sponsorToUpdate->name << "): ";
                    std::cin.ignore(); getline(std::cin, newName);
                    sponsorToUpdate->name = newName;

                    std::cout << "Enter new Amount (current: $" << sponsorToUpdate->amount << "): ";
                    std::cin >> newAmount;
                    if (std::cin.fail() || newAmount <= 0) {
                        std::cout << "Error: Invalid amount. Update cancelled.\n";
                        sponsorToUpdate->name = oldName; // Revert name change
                    }
                    else {
                        sponsorToUpdate->amount = newAmount;

                        std::cout << "Enter new Description (current: " << sponsorToUpdate->description << "): ";
                        std::cin.ignore(); getline(std::cin, newDesc);
                        sponsorToUpdate->description = newDesc;

                        std::string oldTransactionDesc = "Sponsorship: " + oldName;
                        Transaction* transactionToUpdate = nullptr;
                        for (auto& t : transactions) {
                            if (t->description == oldTransactionDesc && t->vendorId == 0) {
                                transactionToUpdate = t.get();
                                break;
                            }
                        }

                        if (transactionToUpdate) {
                            transactionToUpdate->amount = newAmount;
                            transactionToUpdate->description = "Sponsorship: " + newName;
                            std::cout << "\nAssociated transaction updated successfully.\n";
                        }
                        else {
                            std::cout << "\nWarning: Could not find matching transaction to update.\n";
                        }
                        std::cout << "Sponsorship updated successfully!\n";
                    }
                }
                system("pause");
                break;
            }
            case 4: { // Delete
                system("cls");
                std::cout << "--- Delete Sponsorship ---\n";
                int id;
                std::cout << "Enter ID of sponsor to delete: ";
                std::cin >> id;

                auto it = std::find_if(sponsorships.begin(), sponsorships.end(), [id](const auto& s) {
                    return s->sponsorId == id;
                    });

                if (it == sponsorships.end()) {
                    std::cout << "Error: Sponsorship not found.\n";
                }
                else {
                    // Before deleting the sponsorship, delete its transaction
                    std::string transactionDesc = "Sponsorship: " + (*it)->name;
                    auto trans_it = std::find_if(transactions.begin(), transactions.end(), [&](const auto& t) {
                        return t->description == transactionDesc && t->vendorId == 0;
                        });

                    if (trans_it != transactions.end()) {
                        transactions.erase(trans_it);
                        std::cout << "Associated transaction deleted.\n";
                    }
                    else {
                        std::cout << "Warning: No matching transaction found to delete.\n";
                    }

                    sponsorships.erase(it);
                    std::cout << "Sponsorship deleted successfully.\n";
                }
                system("pause");
                break;
            }
            case 0: return;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
            }
        }
    }
}