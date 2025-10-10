#define NOMINMAX
#include "Controller.h"
#include "Menu.h"
#include <iostream>
#include <limits>
#include <windows.h>
#include <memory>
#include "../EntityManagement/VendorManagement.h"
#include "../EntityManagement/BoothManagement.h"
#include "../EntityManagement/TransactionManagement.h"
#include "../EntityManagement/MarketingManagement.h"
#include "../EntityManagement/ProductManagement.h"
#include "../EntityManagement/ReportManagement.h"
#include "../EntityManagement/FeedbackManagement.h"

Controller::Controller(
    std::vector<std::unique_ptr<Vendor>>& v,
    std::vector<std::unique_ptr<Booth>>& b,
    std::vector<std::unique_ptr<Product>>& p,
    std::vector<std::unique_ptr<Transaction>>& t,
    std::vector<std::unique_ptr<Logistics>>& l,
    std::vector<std::unique_ptr<Booking>>& bk,
    std::vector<std::unique_ptr<MarketingCampaign>>& camp,
    std::vector<std::unique_ptr<Sponsorship>>& spon,
    std::vector<std::unique_ptr<Feedback>>& f,
    double& funds,
    int& hype)
    : vendors(v), booths(b), products(p), transactions(t), logistics(l), bookings(bk),
    campaigns(camp), sponsorships(spon), feedbacks(f), eventFunds(funds), eventHype(hype)
{
}

void Controller::run() {
    int choice = -1;
    while (choice != 8) {
        Menu::displayMainMenu(eventFunds, eventHype);
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }
        switch (choice) {
        case 1:
            VendorManagement::manageVendors(vendors, booths, bookings, transactions);
            break;
        case 2:
            BoothManagement::manageBooths(booths, vendors, logistics, bookings, transactions);
            break;
        case 3:
            ProductManagement::manageProducts(products, vendors);
            break;
        case 4:
            MarketingManagement::manageMarketing(campaigns, vendors, products, transactions, sponsorships, eventFunds, eventHype);
            break;
        case 5:
            TransactionManagement::manageTransactions(transactions, vendors);
            break;
        case 6:
            FeedbackManagement::manageFeedbacks(feedbacks, vendors, eventHype);
        case 7:
            ReportManagement::manageReports(vendors, booths, products, transactions, logistics, campaigns, sponsorships, feedbacks);
            break;
        case 8:
            std::cout << "Exit command received. Saving data...\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            system("pause");
            break;
        }
    }
}