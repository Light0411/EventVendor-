#define NOMINMAX
#include "MarketingManagement.h"
#include "../UniversalFunction/GenericUtils.h"
#include "ModuleHelpers.h"
#include "../Entity/MarketingCampaign.h"
#include "../Entity/Vendor.h"
#include "../Entity/Product.h"
#include "../Entity/Transaction.h"
#include "../Entity/Sponsorship.h"
#include "../UI/Menu.h"
#include "SponsorshipManagement.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <limits>
#include <sstream>
#include <map>

// --- Forward declarations for all helper ---
static void displayCampaignDashboard(const std::vector<std::unique_ptr<MarketingCampaign>>&, const std::vector<std::unique_ptr<Vendor>>&, const std::vector<std::unique_ptr<Product>>&, int);
static void sellNewPromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>&, const std::vector<std::unique_ptr<Vendor>>&, const std::vector<std::unique_ptr<Product>>&, std::vector<std::unique_ptr<Transaction>>&, double&, int&);
static void updatePromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>&);
static void cancelPromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>&, std::vector<std::unique_ptr<Transaction>>&, double&, int&);
static void displayVendorHypeRanking(const std::vector<std::unique_ptr<MarketingCampaign>>&, const std::vector<std::unique_ptr<Vendor>>&);
static int getMarketingMenuChoice(const std::vector<std::unique_ptr<Transaction>>&, const std::vector<std::unique_ptr<Sponsorship>>&, const std::vector<std::unique_ptr<MarketingCampaign>>&);

// --- sync crud menus ---
static int getMarketingMenuChoice(const std::vector<std::unique_ptr<Transaction>>& transactions, const std::vector<std::unique_ptr<Sponsorship>>& sponsorships, const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns) {
    system("cls");
    double campaignIncome = 0.0, campaignCost = 0.0, sponsorshipIncome = 0.0;
    for (const auto& t : transactions) {
        if (t->description.find("Pkg") != std::string::npos && t->amount > 0) {
            campaignIncome += t->amount;
        }
    }
    for (const auto& s : sponsorships) { sponsorshipIncome += s->amount; }
    for (const auto& c : campaigns) { campaignCost += c->costToEvent; }
    std::cout << "=============== MARKETING & PROMOTIONS HUB ================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Campaign Income: $" << campaignIncome << "\n";
    std::cout << "Total Campaign Expenses: $" << campaignCost << "\n\n";
    std::cout.unsetf(std::ios_base::floatfield);
    std::cout << "  1. View Sold Promotion Packages \n";
    std::cout << "  2. Sell New Promotion Package \n";
    std::cout << "  3. Update Package Description \n";
    std::cout << "  4. Cancel a Promotion Package \n";
    std::cout << "  5. View Vendor Hype Rankings\n";
    std::cout << "  0. Return to Main Menu\n";
    std::cout << "===========================================================\n";
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); return -1;
    }
    return choice;
}

// --- VIEW / READ FUNCTION ---
static void displayCampaignDashboard(const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns, const std::vector<std::unique_ptr<Vendor>>& vendors, const std::vector<std::unique_ptr<Product>>& products, int eventHype) {
    system("cls");
    std::cout << "================================ SOLD PROMOTIONS DASHBOARD ================================\n";
    std::cout << "  Current Event Hype: " << eventHype << " Points\n";
    std::cout << "=========================================================================================\n\n";
    if (campaigns.empty()) { std::cout << "No promotion packages have been sold yet.\n"; }
    for (const auto& camp : campaigns) {
        std::vector<std::string> details;
        const Vendor* v = GenericUtils::findById(vendors, camp->targetVendorId);
        std::string title = "[ID: " + std::to_string(camp->campaignId) + "] " + camp->campaignType;
        details.push_back("   - Sold to: " + (v ? v->name : "Unknown Vendor"));
        details.push_back("   - Promoting: " + camp->name);
        details.push_back("   - Description: " + camp->description);
        std::stringstream priceLine, costLine;
        priceLine << "   - Price: $" << std::fixed << std::setprecision(2) << camp->package.priceForVendor;
        costLine << "   - Cost: $" << std::fixed << std::setprecision(2) << camp->costToEvent;
        details.push_back(priceLine.str());
        details.push_back(costLine.str());
        size_t max_lines = std::max(camp->asciiArt.size(), details.size() + 2);
        for (size_t i = 0; i < max_lines; ++i) {
            setConsoleColor(camp->colorCode);
            if (i < camp->asciiArt.size()) { std::cout << std::left << std::setw(30) << camp->asciiArt[i]; }
            else { std::cout << std::left << std::setw(30) << " "; }
            setConsoleColor(7);
            if (i == 0) {
                setConsoleColor(camp->colorCode); std::cout << title; setConsoleColor(7);
            }
            else if (i > 0 && (i - 1) < details.size()) {
                std::cout << details[i - 1];
            }
            else if (i == details.size() + 1) {
                setConsoleColor(camp->colorCode);
                std::cout << "   - Hype Gained: " << camp->hypePoints;
                setConsoleColor(7);
            }
            std::cout << "\n";
        }
        std::cout << "-----------------------------------------------------------------------------------------\n";
    }
    system("pause");
}

// --- SELL / CREATE FUNCTION ---
static void sellNewPromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>& campaigns, const std::vector<std::unique_ptr<Vendor>>& vendors, const std::vector<std::unique_ptr<Product>>& products, std::vector<std::unique_ptr<Transaction>>& transactions, double& eventFunds, int& eventHype) {
    system("cls");
    std::cout << "--- Sell New Promotion Package ---\n";
    std::cout << "Enter Vendor ID to sell to: ";
    int vendorId;
    std::cin >> vendorId;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Invalid input. Please enter a number.\n"; system("pause"); return;
    }
    const Vendor* vendor = GenericUtils::findById(vendors, vendorId);
    if (!vendor) { std::cout << "Error: Vendor not found.\n"; system("pause"); return; }
    std::vector<const Product*> vendorProducts;
    for (const auto& p : products) { if (p->vendorId == vendorId) { vendorProducts.push_back(p.get()); } }
    if (vendorProducts.empty()) { std::cout << "This vendor has no products to promote.\n"; system("pause"); return; }
    std::vector<int> selectedProductIds;
    std::string selectedProductNames;
    int firstProductId = 0;
    while (selectedProductIds.size() < 3) {
        system("cls");
        std::cout << "--- Selecting Products for " << vendor->name << " ---\n";
        std::cout << "Selected (" << selectedProductIds.size() << "/3): " << (selectedProductNames.empty() ? "None" : selectedProductNames) << "\n\n";
        std::cout << "Available Products:\n";
        for (const auto* p : vendorProducts) {
            bool isSelected = false;
            for (int id : selectedProductIds) { if (p->productId == id) { isSelected = true; break; } }
            if (!isSelected) { std::cout << "  [ID: " << p->productId << "] " << p->name << "\n"; }
        }
        if (!selectedProductIds.empty()) { std::cout << "\nEnter Product ID to add, or 0 to finish selecting: "; }
        else { std::cout << "\nEnter Product ID to add (must select at least one): "; }
        int productId;
        std::cin >> productId;
        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Error: Invalid input. Please enter a number.\n"; system("pause"); continue;
        }
        if (productId == 0) {
            if (selectedProductIds.empty()) { std::cout << "Error: You must select at least one product.\n"; system("pause"); continue; }
            break;
        }
        const Product* product = GenericUtils::findById(products, productId);
        bool alreadySelected = false;
        for (int id : selectedProductIds) { if (productId == id) { alreadySelected = true; break; } }
        if (!product || product->vendorId != vendorId || alreadySelected) {
            std::cout << "Error: Invalid or already selected Product ID.\n"; system("pause"); continue;
        }
        selectedProductIds.push_back(productId);
        if (!selectedProductNames.empty()) selectedProductNames += ", ";
        selectedProductNames += product->name;
        if (firstProductId == 0) firstProductId = productId;
    }
    std::cout << "\n--- Select Promotion Type ---\n";
    std::cout << "1. Social Media Campaign (Price: $300, Cost: $50, +50 Hype)\n";
    std::cout << "2. Hire Street Team (Price: $500, Cost: $200, +120 Hype)\n";
    std::cout << "3. Online Banner Ads (Price: $1200, Cost: $300, +300 Hype)\n";
    std::cout << "4. TV Commercial Slot (Price: $3000, Cost: $1500, +1000 Hype)\n";
    std::cout << "0. Cancel\n";
    std::cout << "Enter choice: ";
    int promoChoice;
    std::cin >> promoChoice;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Invalid input.\n"; system("pause"); return;
    }
    std::string promoType, promoDesc;
    double price = 0, cost = 0;
    int hype = 0;
    int color = 7;
    std::vector<std::string> art;
    std::vector<std::string> socialArt = { R"(    .--.     )", R"(   |o_o |    )", R"(   |\_/ |    )", R"(  //   \ \   )", R"( (|     | )  )", R"('/\_   _/`\ )", R"(\___)=(___/)" };
    std::vector<std::string> flyerArt = { R"(+----------------+)", R"(|   COME TO THE  |)", R"(| ## EVENT ##   |)", R"(|  [Date Here]   |)", R"(+----------------+)" };
    std::vector<std::string> bannerArt = { R"(+================+)", R"(|  PROMOTION!    |)", R"(|  Check out our |)", R"(|  amazing deal! |)", R"(+================+)" };
    std::vector<std::string> tvArt = { R"(.---------------.)", R"(|[  ___  ] [ TV ]|)", R"(| [_|_]   [___] |)", R"(|_______________|)", R"(\_/)__|_|__(\_/)" };
    switch (promoChoice) {
    case 1: promoType = "Social Media Campaign"; price = 300.0; cost = 50.0; hype = 50; color = 11; art = socialArt; promoDesc = "Targetting Instagram Reels Ads"; break;
    case 2: promoType = "Street Team Promoters"; price = 500.0; cost = 200.0; hype = 120; color = 10; art = flyerArt; promoDesc = "Flyers distributed at high-traffic areas."; break;
    case 3: promoType = "Online Banner Ads"; price = 1200.0; cost = 300.0; hype = 300; color = 13; art = bannerArt; promoDesc = "Online ads on partner websites."; break;
    case 4: promoType = "TV Commercial Slot"; price = 3000.0; cost = 1500.0; hype = 1000; color = 12; art = tvArt; promoDesc = "30-second prime time TV spot."; break;
    case 0: return;
    default: std::cout << "Invalid choice.\n"; system("pause"); return;
    }
    std::stringstream formattedPrice;
    formattedPrice << "$" << std::fixed << std::setprecision(2) << price;
    std::cout << "\nSell '" << promoType << "' package for product(s) '" << selectedProductNames << "' to '" << vendor->name << "' for " << formattedPrice.str() << "? (y/n): ";
    char confirm;
    std::cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        eventFunds += price;
        eventHype += hype;
        int newTransId = transactions.empty() ? 1 : transactions.back()->transactionId + 1;
        std::string desc = promoType + " Pkg for " + selectedProductNames;
        transactions.push_back(std::make_unique<Transaction>(newTransId, vendorId, price, desc));
        int newCampId = campaigns.empty() ? 1 : campaigns.back()->campaignId + 1;
        MarketingCampaign::PackageInfo pkgInfo(price, true);
        auto campaign = std::make_unique<MarketingCampaign>(newCampId, promoType, selectedProductNames, promoDesc, vendorId, firstProductId, hype, color, art, pkgInfo);
        campaign->costToEvent = cost;
        campaigns.push_back(std::move(campaign));
        std::cout << "Package sold successfully!\n";
    }
    else {
        std::cout << "Sale cancelled.\n";
    }
    system("pause");
}

static void updatePromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>& campaigns) {
    system("cls");
    std::cout << "--- Update Promotion Package ---\n";
    std::cout << "Enter Campaign ID to update: ";
    int campId;
    std::cin >> campId;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Invalid input.\n"; system("pause"); return;
    }
    MarketingCampaign* target = nullptr;
    for (auto& camp : campaigns) { if (camp->campaignId == campId) { target = camp.get(); break; } }
    if (!target) {
        std::cout << "Error: Campaign ID not found.\n";
    }
    else {
        std::cout << "Enter new description (current: " << target->description << "): ";
        std::string newDesc;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        getline(std::cin, newDesc);
        target->description = newDesc;
        std::cout << "Description updated successfully!\n";
    }
    system("pause");
}

static void cancelPromotionPackage(std::vector<std::unique_ptr<MarketingCampaign>>& campaigns, std::vector<std::unique_ptr<Transaction>>& transactions, double& eventFunds, int& eventHype) {
    system("cls");
    std::cout << "--- Cancel Promotion Package ---\n";
    std::cout << "Enter Campaign ID to cancel: ";
    int campId;
    std::cin >> campId;
    if (std::cin.fail()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Invalid input.\n"; system("pause"); return;
    }
    auto it = std::find_if(campaigns.begin(), campaigns.end(), [campId](const auto& camp) {
        return camp->campaignId == campId;
        });
    if (it == campaigns.end()) {
        std::cout << "Error: Campaign ID not found.\n";
    }
    else {
        double refundAmount = (*it)->package.priceForVendor;
        int hypeToRemove = (*it)->hypePoints;
        eventFunds -= refundAmount;
        eventHype -= hypeToRemove;
        int newTransId = transactions.empty() ? 1 : transactions.back()->transactionId + 1;
        std::string desc = "Refund: " + (*it)->campaignType + " Pkg";
        transactions.push_back(std::make_unique<Transaction>(newTransId, (*it)->targetVendorId, -refundAmount, desc));
        campaigns.erase(it);
        std::cout << "Campaign " << campId << " cancelled.\n";
        std::cout << "Created refund transaction of $" << std::fixed << std::setprecision(2) << refundAmount << ".\n";
        std::cout << "Removed " << hypeToRemove << " hype points.\n";
    }
    system("pause");
}

static void displayVendorHypeRanking(const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns, const std::vector<std::unique_ptr<Vendor>>& vendors) {
    system("cls");
    std::cout << "--- Vendor Hype Point Rankings ---\n\n";
    std::map<int, int> vendorHypeMap;
    for (const auto& camp : campaigns) {
        if (camp->targetVendorId != 0) {
            vendorHypeMap[camp->targetVendorId] += camp->hypePoints;
        }
    }
    if (vendorHypeMap.empty()) {
        std::cout << "No vendors have generated hype points from campaigns yet.\n";
    }
    else {
        std::vector<std::pair<int, int>> sortedVendors(vendorHypeMap.begin(), vendorHypeMap.end());
        std::sort(sortedVendors.begin(), sortedVendors.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
            });
        const int rank_w = 6, vid_w = 11, vname_w = 25, hype_w = 15;
        std::string border = "+" + std::string(rank_w, '-') + "+" + std::string(vid_w, '-') + "+" + std::string(vname_w, '-') + "+" + std::string(hype_w, '-') + "+\n";
        std::cout << border;
        std::cout << "| " << std::left << std::setw(rank_w - 2) << "Rank" << " | "
            << std::setw(vid_w - 2) << "Vendor ID" << " | "
            << std::setw(vname_w - 2) << "Vendor Name" << " | "
            << std::setw(hype_w - 2) << "Total Hype" << " |\n";
        std::cout << border;
        int rank = 1;
        for (const auto& pair : sortedVendors) {
            int vendorId = pair.first;
            int totalHype = pair.second;
            const Vendor* vendor = GenericUtils::findById(vendors, vendorId);
            std::string vendorName = vendor ? vendor->name : "Unknown ID";
            if (vendorName.length() > vname_w - 2) vendorName = vendorName.substr(0, vname_w - 3) + ".";
            std::cout << "| " << std::left << std::setw(rank_w - 2) << rank++ << " | "
                << std::setw(vid_w - 2) << vendorId << " | "
                << std::setw(vname_w - 2) << vendorName << " | "
                << std::right << std::setw(hype_w - 2) << totalHype << " |\n";
        }
        std::cout << border;
    }
    system("pause");
}

namespace MarketingManagement {
    void manageMarketing(
        std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
        const std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Product>>& products,
        std::vector<std::unique_ptr<Transaction>>& transactions,
        std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        double& eventFunds,
        int& eventHype
    ) {
        int choice = -1;
        while (choice != 0) {
            choice = getMarketingMenuChoice(transactions, sponsorships, campaigns);
            switch (choice) {
            case 1:
                displayCampaignDashboard(campaigns, vendors, products, eventHype);
                break;
            case 2:
                sellNewPromotionPackage(campaigns, vendors, products, transactions, eventFunds, eventHype);
                break;
            case 3:
                updatePromotionPackage(campaigns);
                break;
            case 4:
                cancelPromotionPackage(campaigns, transactions, eventFunds, eventHype);
                break;
            case 5:
                displayVendorHypeRanking(campaigns, vendors);
                break;
            case 0: break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                system("pause");
                break;
            }
        }
    }
}