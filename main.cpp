#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <string>
#include "Entity/Vendor.h"
#include "Entity/Booth.h"
#include "Entity/Product.h"
#include "Entity/Transaction.h"
#include "Entity/Logistics.h"
#include "Entity/Booking.h"
#include "Entity/MarketingCampaign.h"
#include "Entity/Sponsorship.h"
#include "Entity/Feedback.h"
#include "Database/DatabaseManager.h"
#include "UI/Controller.h"
#include "sqlite3.h"

int main() {
	//HELLO MY RESPECTED TUTOR! USE THIS DB YA, PLEASE CHANGE IT TO WHERE YOU EXTRACTED IT TO
    std::string dbPath = "D:\\DCS2S1G2-ChewJinYang-SenZanKai-LiowZhengHao-DennisYapJiaHong\\Source Code\\EventVendorProject\\event_vendor.db";

    DatabaseManager dbManager(dbPath);

    std::vector<std::unique_ptr<Vendor>> vendors;
    std::vector<std::unique_ptr<Booth>> booths;
    std::vector<std::unique_ptr<Product>> products;
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::vector<std::unique_ptr<Logistics>> logistics;
    std::vector<std::unique_ptr<Booking>> bookings;
    std::vector<std::unique_ptr<MarketingCampaign>> campaigns;
    std::vector<std::unique_ptr<Sponsorship>> sponsorships;
    std::vector<std::unique_ptr<Feedback>> feedbacks;
    dbManager.loadAllData(vendors, booths, products, transactions, logistics, bookings, sponsorships, campaigns, feedbacks);


	// Initialize event funds
    // base capital
    double eventFunds = 2000.0; 
    // Add 15% of total sponsorship amounts to initial capital (Sponsorship.amount is public)
    double sponsorshipTotal = 0.0;
    for (const auto& s : sponsorships) {
        sponsorshipTotal += s->amount;
    }
    eventFunds += sponsorshipTotal * 0.85;


	// Initialize event hype
    int eventHype = 0;
    const int baseHype = 100;           
    int hypeFromCampaigns = 0;
	// Count all active campaign hype points
    for (const auto& c : campaigns) {
        if (c->status == "Active") {    
            hypeFromCampaigns += c->hypePoints;
        }
    }
    eventHype = baseHype + hypeFromCampaigns;


    Controller appController(
        vendors, booths, products, transactions, logistics, bookings,
        campaigns, sponsorships, feedbacks, eventFunds, eventHype
    );
    appController.run();

    // Persist back to DB
    dbManager.saveAllData(vendors, booths, products, transactions, logistics, bookings, sponsorships, campaigns, feedbacks);

    std::cout << "Press enter to exit.";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    return 0;
}
