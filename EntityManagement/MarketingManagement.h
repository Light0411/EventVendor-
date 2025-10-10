#pragma once
#include <vector>
#include <string>
#include <memory>

struct MarketingCampaign;
struct Vendor;
struct Product;
struct Transaction;
struct Sponsorship;

namespace MarketingManagement {
    void manageMarketing(
        std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
        const std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Product>>& products,
        std::vector<std::unique_ptr<Transaction>>& transactions,
        std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        double& eventFunds,
        int& eventHype
    );
}