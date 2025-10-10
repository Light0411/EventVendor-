#define NOMINMAX
#include "ProductManagement.h"
#include "ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Product.h"
#include "../Entity/Vendor.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>

static void displayProductsTable(const std::vector<Product*>& productList, const std::vector<std::unique_ptr<Vendor>>& allVendors, const std::string& title) {
    system("cls");
    std::cout << "--- " << title << " ---\n\n";
    if (productList.empty()) {
        std::cout << "No products to display.\n";
    }
    else {
        const int id_w = 12, vid_w = 11, vname_w = 18, name_w = 24, price_w = 14;
        std::stringstream border;
        border << "+" << std::string(id_w + 2, '-') << "+" << std::string(vid_w + 2, '-') << "+" << std::string(vname_w + 2, '-') << "+" << std::string(name_w + 2, '-') << "+" << std::string(price_w + 2, '-') << "+";
        std::cout << border.str() << "\n";
        std::cout << "| " << std::left << std::setw(id_w) << "Product ID" << " "
            << "| " << std::setw(vid_w) << "Vendor ID" << " "
            << "| " << std::setw(vname_w) << "Vendor Name" << " "
            << "| " << std::setw(name_w) << "Product Name" << " "
            << "| " << std::setw(price_w) << "Price" << " |\n";
        std::cout << border.str() << "\n";

        for (const auto* product : productList) {
            const Vendor* vendor = GenericUtils::findById(allVendors, product->vendorId);
            std::string vendorName = vendor ? vendor->name : "Unknown";
            if (vendorName.length() > vname_w) vendorName = vendorName.substr(0, vname_w - 1) + ".";

            std::string prodName = product->name;
            if (prodName.length() > name_w) prodName = prodName.substr(0, name_w - 1) + ".";

            std::cout << "| " << std::left << std::setw(id_w) << product->productId << " "
                << "| " << std::setw(vid_w) << product->vendorId << " "
                << "| " << std::setw(vname_w) << vendorName << " "
                << "| " << std::setw(name_w) << prodName << " "
                << "| $" << std::right << std::setw(price_w - 1) << std::fixed << std::setprecision(2) << product->price << " |\n";
        }
        std::cout << border.str() << "\n";
    }
}

namespace ProductManagement {

    void manageProducts(std::vector<std::unique_ptr<Product>>& products, const std::vector<std::unique_ptr<Vendor>>& allVendors) {
        int choice = -1;
        while (choice != 0) {
            choice = getSubMenuChoice("Products");

            switch (choice) {
            case 1: { // Add New Product
                system("cls");
                std::cout << "--- Add New Product ---\n";
                int prodId, vendorId;
                std::string name;
                double price;

                std::cout << "Enter Product ID: ";
                std::cin >> prodId;
                if (std::cin.fail() || GenericUtils::findById(products, prodId) != nullptr) {
                    std::cout << "Error: Invalid or duplicate Product ID.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                std::cout << "Enter Vendor ID to associate this product with: ";
                std::cin >> vendorId;
                if (std::cin.fail() || GenericUtils::findById(allVendors, vendorId) == nullptr) {
                    std::cout << "Error: Vendor ID not found.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                std::cout << "Enter Product Name: ";
                std::cin.ignore();
                getline(std::cin, name);

                std::cout << "Enter Price: ";
                std::cin >> price;
                if (std::cin.fail() || price < 0) {
                    std::cout << "Error: Invalid or negative price input.\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                products.push_back(std::make_unique<Product>(prodId, vendorId, name, price));
                std::cout << "Product added successfully!\n";
                system("pause");
                break;
            }
            case 2: { // View Products
                int viewChoice = -1;
                std::vector<Product*> productPtrs;
                for (const auto& p : products) {
                    productPtrs.push_back(p.get());
                }
                std::string currentTitle = "All Registered Products";
                bool resetView = false;

                while (viewChoice != 0) {
                    system("cls");

                    if (resetView) {
                        productPtrs.clear();
                        for (const auto& p : products) { productPtrs.push_back(p.get()); }
                        currentTitle = "All Registered Products";
                        resetView = false;
                    }

                    displayProductsTable(productPtrs, allVendors, currentTitle);

                    std::cout << "\n--- Options ---\n";
                    std::cout << "1. Sort by Name (A-Z)\n";
                    std::cout << "2. Sort by Price (Low to High)\n";
                    std::cout << "3. Sort by Price (High to Low)\n";
                    std::cout << "4. Find Products by Vendor\n";
                    std::cout << "5. Reset View\n";
                    std::cout << "0. Back to Product Menu\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> viewChoice;
                    if (std::cin.fail()) {
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        viewChoice = -1;
                    }

                    switch (viewChoice) {
                    case 1:
                        std::sort(productPtrs.begin(), productPtrs.end(), [](const Product* a, const Product* b) { return a->name < b->name; });
                        currentTitle = "Products Sorted by Name";
                        break;
                    case 2:
                        std::sort(productPtrs.begin(), productPtrs.end(), [](const Product* a, const Product* b) { return a->price < b->price; });
                        currentTitle = "Products Sorted by Price (Low-High)";
                        break;
                    case 3:
                        std::sort(productPtrs.begin(), productPtrs.end(), [](const Product* a, const Product* b) { return a->price > b->price; });
                        currentTitle = "Products Sorted by Price (High-Low)";
                        break;
                    case 4: {
                        std::cout << "Enter Vendor ID or full Vendor Name: ";
                        std::string input;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        getline(std::cin, input);

                        const Vendor* foundVendor = nullptr;
                        try {
                            int vendorId = std::stoi(input);
                            foundVendor = GenericUtils::findById(allVendors, vendorId);
                        }
                        catch (const std::exception&) {
                            for (const auto& v : allVendors) { if (v->name == input) { foundVendor = v.get(); break; } }
                        }

                        if (!foundVendor) {
                            std::cout << "Error: Vendor not found.\n";
                            system("pause");
                        }
                        else {
                            productPtrs.erase(std::remove_if(productPtrs.begin(), productPtrs.end(),
                                [foundVendor](const Product* p) { return p->vendorId != foundVendor->vendorId; }), productPtrs.end());
                            currentTitle = "Products for Vendor: " + foundVendor->name;
                        }
                        break;
                    }
                    case 5:
                        resetView = true;
                        break;
                    case 0: break;
                    default:
                        std::cout << "Invalid view choice.\n";
                        system("pause");
                        break;
                    }
                }
                break;
            }
            case 3: { // Update Product
                system("cls");
                std::cout << "--- Update Product ---\n";
                int prodId;
                std::cout << "Enter ID of product to update: ";
                std::cin >> prodId;
                Product* product = GenericUtils::findById(products, prodId);
                if (product == nullptr) {
                    std::cout << "Error: Product not found.\n";
                }
                else {
                    std::string newName;
                    double newPrice;
                    std::cout << "Enter new Name (current: " << product->name << "): ";
                    std::cin.ignore(); getline(std::cin, newName);
                    product->name = newName;
                    std::cout << "Enter new Price (current: " << product->price << "): ";
                    std::cin >> newPrice;
                    if (std::cin.fail()) {
                        std::cout << "Error: Invalid price input.\n";
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    else {
                        product->price = newPrice;
                        std::cout << "Product updated successfully!\n";
                    }
                }
                system("pause");
                break;
            }
            case 4: { // Delete Product
                system("cls");
                std::cout << "--- Delete Product ---\n";
                int id;
                std::cout << "Enter ID of product to delete: ";
                std::cin >> id;
                auto it = std::find_if(products.begin(), products.end(), [id](const std::unique_ptr<Product>& p) {
                    return p->productId == id;
                    });
                if (it == products.end()) {
                    std::cout << "Error: Product not found.\n";
                }
                else {
                    products.erase(it);
                    std::cout << "Product deleted successfully.\n";
                }
                system("pause");
                break;
            }
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