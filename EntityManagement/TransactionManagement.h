#pragma once
#include <vector>
#include <string>
#include <memory>

struct Transaction;
struct Vendor;

namespace TransactionManagement {
    void manageTransactions(std::vector<std::unique_ptr<Transaction>>& transactions, const std::vector<std::unique_ptr<Vendor>>& allVendors);
}