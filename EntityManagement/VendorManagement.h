#pragma once
#include <vector>
#include <string>
#include <memory>

struct Booth;
struct Vendor;
struct Booking;
struct Transaction;

namespace VendorManagement {
    void manageVendors(
        std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Booth>>& booths,
        std::vector<std::unique_ptr<Booking>>& bookings,
        std::vector<std::unique_ptr<Transaction>>& transactions
    );
}