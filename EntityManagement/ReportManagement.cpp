#define NOMINMAX
#include "ReportManagement.h"
#include "../EntityManagement/ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Vendor.h"
#include "../Entity/Booth.h"
#include "../Entity/Product.h"
#include "../Entity/Transaction.h"
#include "../Entity/Logistics.h"
#include "../Entity/MarketingCampaign.h"
#include "../Entity/Sponsorship.h"
#include "../Entity/Feedback.h"
#include "SponsorshipManagement.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>
#include <numeric>
#include <map>
#include <algorithm>
#include <limits>
#include <sstream>
#include <set>
#include <cctype>
#include <unordered_map>

static void generateFinancialStatement(
    const std::vector<std::unique_ptr<Transaction>>&,
    const std::vector<std::unique_ptr<Logistics>>&,
    const std::vector<std::unique_ptr<MarketingCampaign>>&,
    const std::vector<std::unique_ptr<Sponsorship>>&
);
static void generateOperationalDashboard(
    const std::vector<std::unique_ptr<Vendor>>&,
    const std::vector<std::unique_ptr<Booth>>&,
    const std::vector<std::unique_ptr<Transaction>>&,
    const std::vector<std::unique_ptr<MarketingCampaign>>&,
    const std::vector<std::unique_ptr<Sponsorship>>&
);

static void generateFeedbackReport(const std::vector<std::unique_ptr<Feedback>>& feedbacks);

static void generateKpiReport(const std::vector<std::unique_ptr<Vendor>>&, 
    const std::vector<std::unique_ptr<Product>>&, 
    const std::vector<std::unique_ptr<Transaction>>&, 
    const std::vector<std::unique_ptr<MarketingCampaign>>&,
    const std::vector<std::unique_ptr<Feedback>>&
);


namespace ReportManagement {
    void manageReports(
        const std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Booth>>& booths,
        const std::vector<std::unique_ptr<Product>>& products,
        std::vector<std::unique_ptr<Transaction>>& transactions,
        const std::vector<std::unique_ptr<Logistics>>& logistics,
        const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
        std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        const std::vector<std::unique_ptr<Feedback>>& feedbacks
    ) {
        int choice = -1;
        while (choice != 0) {
            system("cls");
            std::cout << "\n--- Reporting Suite ---\n";
            std::cout << "1. Generate Financial Statement (P&L)\n";
            std::cout << "2. Generate Operational Dashboard\n";
            std::cout << "3. Manage Sponsorships\n";
            std::cout << "4. Generate Feedback Report\n";
            std::cout << "5. Generate KPI Summary Report\n";
            std::cout << "0. Return to Main Menu\n";
            std::cout << "-------------------------------------\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                choice = -1;
            }
            switch (choice) {
            case 1:
                generateFinancialStatement(transactions, logistics, campaigns, sponsorships);
                break;
            case 2:
                generateOperationalDashboard(vendors, booths, transactions, campaigns, sponsorships);
                break;
            case 3:
                SponsorshipManagement::manageSponsorships(sponsorships, const_cast<std::vector<std::unique_ptr<Transaction>>&>(transactions)); // Allow sponsorships to modify transactions
                break;
            case 4:
                generateFeedbackReport(feedbacks);
                break;
            case 5:
                generateKpiReport(vendors, products, transactions, campaigns, feedbacks);
                break;
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

static void generateFinancialStatement(
    const std::vector<std::unique_ptr<Transaction>>& transactions,
    const std::vector<std::unique_ptr<Logistics>>& logistics,
    const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
    const std::vector<std::unique_ptr<Sponsorship>>& sponsorships
) {
    system("cls");
    std::cout << "====================================================\n";
    std::cout << "           INCOME STATEMENT (PROFIT & LOSS)\n";
    std::cout << "====================================================\n\n";

    double boothRevenue = 0.0, marketingRevenue = 0.0;
    for (const auto& t : transactions) { // Sum up revenues from transactions
        if (t->description.find("Pkg") != std::string::npos && t->amount > 0) {
            marketingRevenue += t->amount;
        }
        if (t->description.find("Booth Rental Fee") != std::string::npos) { // Booth rental revenue
            boothRevenue += t->amount;
        }
    }

    double sponsorshipRevenue = 0.0;
    for (const auto& s : sponsorships) { // Sum up sponsorship revenues
        sponsorshipRevenue += s->amount;
    }

    double totalRevenue = boothRevenue + marketingRevenue + sponsorshipRevenue;

    double logisticsCosts = 0.0;
    for (const auto& l : logistics) { // Sum up logistics costs
        logisticsCosts += l->getTotalCost();
    }

    double marketingCosts = 0.0;
    for (const auto& c : campaigns) { // Sum up marketing campaign costs
        marketingCosts += c->costToEvent;
    }

    double totalDirectCosts = logisticsCosts + marketingCosts;
    double netProfit = totalRevenue - totalDirectCosts;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "--- Revenue ---\n";
    std::cout << "  Booth Booking Revenue:              $" << std::setw(10) << boothRevenue << "\n";
    std::cout << "  Marketing Package Revenue:          $" << std::setw(10) << marketingRevenue << "\n";
    std::cout << "  Sponsorship Revenue:                $" << std::setw(10) << sponsorshipRevenue << "\n";
    std::cout << "  --------------------------------------------------\n";
    std::cout << "  Total Revenue:                      $" << std::setw(10) << totalRevenue << "\n\n";

    std::cout << "--- Direct Costs & Expenses ---\n";
    std::cout << "  Logistics & Equipment Costs:        $" << std::setw(10) << logisticsCosts << "\n";
    std::cout << "  Marketing Campaign Costs:           $" << std::setw(10) << marketingCosts << "\n";
    std::cout << "  --------------------------------------------------\n";
    std::cout << "  Total Costs & Expenses:             $" << std::setw(10) << totalDirectCosts << "\n\n";

    std::cout << "====================================================\n";
    std::cout << "  NET PROFIT / (LOSS):                $" << std::setw(10) << netProfit << "\n";
    std::cout << "====================================================\n\n";
    system("pause");
}

static void generateOperationalDashboard(
    const std::vector<std::unique_ptr<Vendor>>& vendors,
    const std::vector<std::unique_ptr<Booth>>& booths,
    const std::vector<std::unique_ptr<Transaction>>& transactions,
    const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
    const std::vector<std::unique_ptr<Sponsorship>>& sponsorships
) {
    system("cls");
    std::cout << "====================================================\n";
    std::cout << "        OPERATIONAL & VENDOR DASHBOARD\n";
    std::cout << "====================================================\n\n";

    std::cout << "--- Booth Performance ---\n";
    if (booths.empty()) {
        std::cout << "  No booth data available.\n\n";
    }
    else { // Calculate overall and categorized occupancy rates
        int totalOccupied = 0;
        std::map<std::string, std::pair<int, int>> byType;
        std::map<std::string, std::pair<int, int>> byLocation;
        for (const auto& b : booths) {
            byType[b->type].first++; // total booths of standard type
            byLocation[b->mallName].first++; // total booths at this location
            if (b->assignedVendorId != 0) {
                totalOccupied++; // occupied booths
                byType[b->type].second++; //  total booths of premium type
                byLocation[b->mallName].second++; // total booths at this location
            }
        }
        double overallRate = (booths.size() > 0) ? (static_cast<double>(totalOccupied) / booths.size()) * 100.0 : 0.0; // overall occupancy rate
        std::cout << "  Total Booths: " << booths.size() << " | Occupied: " << totalOccupied << " (" << std::fixed << std::setprecision(1) << overallRate << "%)\n\n";
        std::cout << "  Occupancy by Type:\n";
        for (const auto& pair : byType) { // occupancy rate by booth type
            double rate = (pair.second.first > 0) ? (static_cast<double>(pair.second.second) / pair.second.first) * 100.0 : 0.0;
            std::cout << "    - " << std::left << std::setw(10) << pair.first << ": " << pair.second.second << " / " << pair.second.first << " (" << std::fixed << std::setprecision(1) << rate << "%)\n";
        }
        std::cout << "\n  Occupancy by Location:\n";
        for (const auto& pair : byLocation) { // occupancy rate by mall location
            double rate = (pair.second.first > 0) ? (static_cast<double>(pair.second.second) / pair.second.first) * 100.0 : 0.0;
            std::cout << "    - " << std::left << std::setw(10) << pair.first << ": " << pair.second.second << " / " << pair.second.first << " (" << std::fixed << std::setprecision(1) << rate << "%)\n";
        }
    }

    std::cout << "\n--- Vendor Performance ---\n";
    if (vendors.empty()) {
        std::cout << "  No vendor data available.\n\n";
    }
    else {
        int featuredCount = 0;
        for (const auto& v : vendors) if (v->isFeatured) featuredCount++; // count featured vendors
        std::cout << "  Total Vendors: " << vendors.size() << " | Featured: " << featuredCount << "\n\n";
        std::map<int, double> vendorSpend;
        for (const auto& t : transactions) { // sum up spend per vendor from transactions
            vendorSpend[t->vendorId] += t->amount;
        }
        std::vector<std::pair<int, double>> sortedSpend; // sort vendors by spend amount
        for (const auto& pair : vendorSpend) { sortedSpend.push_back(pair); }
        std::sort(sortedSpend.begin(), sortedSpend.end(), [](const auto& a, const auto& b) { // descending order
            return a.second > b.second;
            });
        std::cout << "  Top 3 Vendors by Spend:\n";
        int count = 0;
        for (const auto& pair : sortedSpend) { // display top 3 vendors
            if (count++ >= 3) break;
            const Vendor* v = GenericUtils::findById(vendors, pair.first);
            std::string vName = v ? v->name : "Unknown ID";
            std::cout << "    " << count << ". " << std::left << std::setw(20) << vName << "$" << std::right << std::setw(10) << std::fixed << std::setprecision(2) << pair.second << "\n";
        }
    }

    std::cout << "\n--- Sponsorship Highlights ---\n";
    if (sponsorships.empty()) {
        std::cout << "  No sponsors for this event.\n";
    }
    else { // Sort sponsors by contribution amount
        std::vector<Sponsorship*> sortedSponsors;
        for (const auto& s : sponsorships) { sortedSponsors.push_back(s.get()); } // raw pointers for sorting
        std::sort(sortedSponsors.begin(), sortedSponsors.end(), [](const auto* a, const auto* b) { // descending order
            return a->amount > b->amount;
            });
        std::cout << "  Top 3 Sponsors by Contribution:\n";
        int count = 0;
        for (const auto* s : sortedSponsors) {
            if (count++ >= 3) break;
            std::cout << "    " << count << ". " << std::left << std::setw(20) << s->name << "$" << std::right << std::setw(10) << std::fixed << std::setprecision(2) << s->amount << "\n";
        }
    }

    std::cout << "\n--- Marketing Efficiency ---\n";
    if (campaigns.empty()) {
        std::cout << "  No sold marketing packages to analyze.\n";
    }
    else { // Determine top-selling marketing package type
        std::map<std::string, int> campaignSales;
        for (const auto& c : campaigns) { // count sales per campaign type
            campaignSales[c->campaignType]++;
        }
        std::string topSeller = "N/A";
        int maxSales = 0;
        for (const auto& pair : campaignSales) { // find type with max sales
            if (pair.second > maxSales) {
                maxSales = pair.second;
                topSeller = pair.first;
            }
        }
        std::cout << "  Top Selling Marketing Package:\n";
        std::cout << "    - '" << topSeller << "' (Sold " << maxSales << " times)\n";
    }

    std::cout << "\n====================================================\n";
    system("pause");
}

static void generateFeedbackReport(const std::vector<std::unique_ptr<Feedback>>& feedbacks) {
    system("cls");
    std::cout << "====================================================\n";
    std::cout << "                 FEEDBACK ANALYSIS REPORT\n";
    std::cout << "====================================================\n\n";

    if (feedbacks.empty()) {
        std::cout << "No feedback data available to generate a report.\n";
        system("pause");
        return;
    }

    // --- Highest Rated Feedback ---
    const Feedback* highestRated = nullptr;
    for (const auto& f : feedbacks) {
        if (!highestRated || f->rating > highestRated->rating) {
            highestRated = f.get();
        }
    }

    if (highestRated) {
        std::cout << "--- Highest Rated Feedback ---\n";
        std::cout << "  Rating: " << highestRated->rating << "/5\n";
        std::cout << "  Vendor ID: " << highestRated->vendorId << "\n";
        std::cout << "  Comment: \"" << highestRated->comment << "\"\n\n";
    }

    // --- Word Frequency Analysis ---
    std::cout << "--- Common Feedback Keywords ---\n";
    std::map<std::string, int> wordCounts;
    std::set<std::string> stopwords = { "a", "an", "the", "is", "was", "in", "it", "of", "and", "to", "for", "with", "on", "very" };

    for (const auto& f : feedbacks) {
        std::stringstream ss(f->comment);
        std::string word;
        while (ss >> word) {
            // Basic text cleaning: lowercase and remove punctuation
            std::transform(word.begin(), word.end(), word.begin(),
                [](unsigned char c) { return std::tolower(c); });

            word.erase(std::remove_if(word.begin(), word.end(),
                [](unsigned char c) { return std::ispunct(c); }),
                word.end());


            if (!word.empty() && stopwords.find(word) == stopwords.end()) {
                wordCounts[word]++;
            }
        }
    }

    if (wordCounts.empty()) {
        std::cout << "  No significant keywords found in comments.\n";
    }
    else {
        std::vector<std::pair<std::string, int>> sortedWords(wordCounts.begin(), wordCounts.end());
        std::sort(sortedWords.begin(), sortedWords.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
            });

        std::cout << "  Top 5 most mentioned words:\n";
        int count = 0;
        for (const auto& pair : sortedWords) {
            if (count++ >= 5) break;
            std::cout << "    " << count << ". " << std::left << std::setw(15) << pair.first << " (mentioned " << pair.second << " times)\n";
        }
    }

    std::cout << "\n====================================================\n";
    system("pause");
}

static void generateKpiReport(
    const std::vector<std::unique_ptr<Vendor>>& allVendors,
    const std::vector<std::unique_ptr<Product>>& allProducts,
    const std::vector<std::unique_ptr<Transaction>>& allTransactions,
    const std::vector<std::unique_ptr<MarketingCampaign>>& allCampaigns, // Added Campaigns for product analysis
    const std::vector<std::unique_ptr<Feedback>>& allFeedbacks
) {
    system("cls");
    std::cout << "====================================================\n";
    std::cout << "             KEY PERFORMANCE INDICATORS\n";
    std::cout << "====================================================\n\n";

    // --- 1. KPIs ---
    double boothRevenue = 0.0;
    double marketingRevenue = 0.0;
    int orderCount = 0;
    std::set<int> activeVendorIds;

    for (const auto& t : allTransactions) {
        if (t->description.find("Booth Rental Fee") != std::string::npos) {
            boothRevenue += t->amount;
        }
        else if (t->description.find("Pkg for") != std::string::npos) {
            marketingRevenue += t->amount;
        }

        if (t->amount > 0) { // Count all positive transactions as "orders"
            orderCount++;
            if (t->vendorId != 0) {
                activeVendorIds.insert(t->vendorId);
            }
        }
    }

    double totalRevenue = boothRevenue + marketingRevenue;
    double aov = (orderCount > 0) ? totalRevenue / orderCount : 0.0;
    int activeVendors = activeVendorIds.size();

    double totalRating = 0;
    for (const auto& f : allFeedbacks) {
        totalRating += f->rating;
    }
    double avgRating = (!allFeedbacks.empty()) ? totalRating / allFeedbacks.size() : 0.0;

    std::cout << "--- KPIs ---\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  - Total Revenue (Vendor Sales): $" << totalRevenue << "\n";
    std::cout << "  - Total Orders:                 " << orderCount << "\n";
    std::cout << "  - Avg. Order Value:             $" << aov << "\n";
    std::cout << "  - Active Vendors:               " << activeVendors << "\n";
    std::cout << "  - Average Rating:               " << avgRating << " / 5.00\n\n";

    // --- 2. Top Lists ---
    std::cout << "--- Top Lists ---\n";

    // Top 5 Vendors by Revenue 
    std::map<int, double> vendorRevenue;
    for (const auto& t : allTransactions) {
        if (t->vendorId != 0 && t->amount > 0) {
            vendorRevenue[t->vendorId] += t->amount;
        }
    }
    std::vector<std::pair<int, double>> sortedVendors(vendorRevenue.begin(), vendorRevenue.end());
    std::sort(sortedVendors.begin(), sortedVendors.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "  Top 5 Vendors by Revenue:\n";
    int rank = 1;
    for (const auto& pair : sortedVendors) {
        if (rank > 5) break;
        const Vendor* v = GenericUtils::findById(allVendors, pair.first);
        std::cout << "    " << rank++ << ". " << std::left << std::setw(20) << (v ? v->name : "Unknown") << " $" << pair.second << "\n";
    }

    // Top 5 Products by Marketing Value
    std::map<int, double> productMarketingValue;
    for (const auto& camp : allCampaigns) {
        if (camp->targetProductId != 0) {
            // Value is the price the vendor paid for the campaign
            productMarketingValue[camp->targetProductId] += camp->package.priceForVendor;
        }
    }
    std::vector<std::pair<int, double>> sortedProducts(productMarketingValue.begin(), productMarketingValue.end());
    std::sort(sortedProducts.begin(), sortedProducts.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "\n  Top 5 Products by Marketing Spend:\n";
    rank = 1;
    for (const auto& pair : sortedProducts) {
        if (rank > 5) break;
        const Product* p = GenericUtils::findById(allProducts, pair.first);
        std::cout << "    " << rank++ << ". " << std::left << std::setw(25) << (p ? p->name : "Unknown Product") << " $" << pair.second << "\n";
    }

    // 3. Rating Distribution 
    std::cout << "\n--- Rating Distribution ---\n";
    std::map<int, int> ratingCounts;
    for (const auto& f : allFeedbacks) {
        ratingCounts[f->rating]++;
    }
    for (int i = 5; i >= 1; --i) {
        std::cout << "  - " << i << " Star" << (i > 1 ? "s" : " ") << ": " << std::string(ratingCounts[i], '*') << " (" << ratingCounts[i] << ")\n";
    }

    // 4. Simple Forecast 
    std::cout << "\n--- Tiny Forecast ---\n";
    const double alpha = 0.3;
    double last_forecast = totalRevenue * 0.9; // Simple assumption for previous period
    double next_forecast = (alpha * totalRevenue) + ((1 - alpha) * last_forecast);
    std::cout << "  - Next Period Revenue Forecast (Exponential Smoothing): $" << next_forecast << "\n";

    std::cout << "\n====================================================\n";
    system("pause");
}