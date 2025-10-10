#pragma once
#include <vector>
#include <memory>

struct Vendor;
struct Booth;
struct Product;
struct Transaction;
struct Logistics;
struct Booking;
struct MarketingCampaign;
struct Sponsorship;
struct Feedback;

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
    );
}