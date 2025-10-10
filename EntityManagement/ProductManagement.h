#pragma once
#include <vector>
#include <string>
#include <memory>

struct Product;
struct Vendor;

namespace ProductManagement {
    void manageProducts(std::vector<std::unique_ptr<Product>>& products, const std::vector<std::unique_ptr<Vendor>>& allVendors);
}