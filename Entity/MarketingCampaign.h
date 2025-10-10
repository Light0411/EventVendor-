#pragma once
#include <string>
#include <vector>

struct MarketingCampaign {
    // --- NESTED STRUCTURE USAGE HERE ---
    struct PackageInfo {
        double priceForVendor;
        bool generatesDirectRevenue;
        PackageInfo(double price = 0.0, bool generatesRevenue = false)
            : priceForVendor(price), generatesDirectRevenue(generatesRevenue) {
        }
    };

    int campaignId;
    std::string campaignType;
    std::string name;
    std::string description;
    double costToEvent;
    int hypePoints;
    std::string status;

    PackageInfo package;

    int targetVendorId;
    int targetProductId;

    std::vector<std::string> asciiArt;
    int colorCode;

    MarketingCampaign(int id, const std::string& n, const std::string& desc, double cost, int hype, int color, const std::vector<std::string>& art)
        : campaignId(id), campaignType("General Hype"), name(n), description(desc), costToEvent(cost), hypePoints(hype),
        status("Active"), colorCode(color), asciiArt(art),
        package(0.0, false), targetVendorId(0), targetProductId(0)
    {
    }

    MarketingCampaign(int id, const std::string& type, const std::string& n, const std::string& desc, int vId, int pId, int hype, int color, const std::vector<std::string>& art, const PackageInfo& pkgInfo)
        : campaignId(id), campaignType(type), name(n), description(desc), costToEvent(0), hypePoints(hype),
        status("Active"), colorCode(color), asciiArt(art),
        package(pkgInfo), targetVendorId(vId), targetProductId(pId)
    {
    }
};