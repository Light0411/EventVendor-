#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Product {
    int productId;
    int vendorId;
    std::string name;
    double price;

    Product(int id, int vId, const std::string& n, double p)
        : productId(id), vendorId(vId), name(n), price(p) {
    }

    void serialize(std::ostream& os) const {
        os << productId << "," << vendorId << "," << name << "," << price;
    }

    static Product deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field, name;
        int id, vId;
        double p;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, field, ','); vId = std::stoi(field);
        getline(ss, name, ',');
        getline(ss, field, ','); p = std::stod(field);

        return Product(id, vId, name, p);
    }
};