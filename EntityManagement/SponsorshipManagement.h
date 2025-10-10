#pragma once
#include <vector>
#include <memory>

struct Sponsorship;
struct Transaction;

namespace SponsorshipManagement {
    void manageSponsorships(
        std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        std::vector<std::unique_ptr<Transaction>>& transactions
    );
}