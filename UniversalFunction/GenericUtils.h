#pragma once
#include <vector>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <string> 
#include <map>    

// foward structs
struct Booth;
struct Vendor;
struct Product;
struct Transaction;

namespace GenericUtils { 

	template <typename T, typename IDType> // ID Type can be int or string based on entity
	T* findById(const std::vector<std::unique_ptr<T>>& items, IDType id) { // Returns raw pointer or nullptr if not found
		auto it = std::find_if(items.begin(), items.end(), [id](const std::unique_ptr<T>& item) { // Lambda to compare IDs
			if constexpr (std::is_same_v<T, Booth>) { return item->boothId == id; } 
            else if constexpr (std::is_same_v<T, Vendor>) { return item->vendorId == id; }
            else if constexpr (std::is_same_v<T, Product>) { return item->productId == id; }
            else if constexpr (std::is_same_v<T, Transaction>) { return item->transactionId == id; }
            return false;
            });
        return (it != items.end()) ? it->get() : nullptr; 
    }

	template <typename T> // Sorts vector of unique_ptr<T> in-place by their ID
	void sortById(std::vector<std::unique_ptr<T>>& items) { // In-place sort
		std::sort(items.begin(), items.end(), [](const std::unique_ptr<T>& a, const std::unique_ptr<T>& b) { // Lambda to compare IDs
            if constexpr (std::is_same_v<T, Booth>) { return a->boothId < b->boothId; }
            else if constexpr (std::is_same_v<T, Vendor>) { return a->vendorId < b->vendorId; }
            else if constexpr (std::is_same_v<T, Product>) { return a->productId < b->productId; }
            else if constexpr (std::is_same_v<T, Transaction>) { return a->transactionId < b->transactionId; }
            return false;
            });
    }

    // Creates a map for fast ID-to-Name lookups.
	template <typename T> // Only works for types that have 'vendorId' and 'name' members
	std::map<int, std::string> createIdToNameMap(const std::vector<std::unique_ptr<T>>& items) { // Returns map of ID to Name
		std::map<int, std::string> idToNameMap; 
        for (const auto& item : items) { 
			if constexpr (std::is_same_v<T, Vendor>) { // Only Vendor has vendorId and name
				idToNameMap[item->vendorId] = item->name; // Map vendorId to name
            }
        }
        return idToNameMap; 
    }
}