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

class Controller {
private:
    std::vector<std::unique_ptr<Vendor>>& vendors;
    std::vector<std::unique_ptr<Booth>>& booths;
    std::vector<std::unique_ptr<Product>>& products;
    std::vector<std::unique_ptr<Transaction>>& transactions;
    std::vector<std::unique_ptr<Logistics>>& logistics;
    std::vector<std::unique_ptr<Booking>>& bookings;
    std::vector<std::unique_ptr<MarketingCampaign>>& campaigns;
    std::vector<std::unique_ptr<Sponsorship>>& sponsorships;
    std::vector<std::unique_ptr<Feedback>>& feedbacks;
    double& eventFunds;
    int& eventHype;

public:
    Controller(
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
        int& hype
    );

    void run();
};