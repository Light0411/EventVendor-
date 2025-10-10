#pragma once
#include <vector>
#include <string>
#include <memory>

struct Booth;
struct Vendor;
struct Logistics;
struct Booking;
struct Transaction; 

namespace BoothManagement {
    void manageBooths(
        std::vector<std::unique_ptr<Booth>>& allBooths,
        const std::vector<std::unique_ptr<Vendor>>& allVendors,
        std::vector<std::unique_ptr<Logistics>>& allLogistics,
        std::vector<std::unique_ptr<Booking>>& allBookings,
        std::vector<std::unique_ptr<Transaction>>& transactions
    );
}