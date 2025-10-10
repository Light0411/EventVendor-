#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Transaction {
    int transactionId;
    int vendorId;
    double amount;
    std::string description;

    Transaction(int id, int vId, double amt, const std::string& desc)
        : transactionId(id), vendorId(vId), amount(amt), description(desc) {
    }

    void serialize(std::ostream& os) const {
        os << transactionId << "," << vendorId << "," << amount << "," << description;
    }

    static Transaction deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field, description;
        int id, vId;
        double amt;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, field, ','); vId = std::stoi(field);
        getline(ss, field, ','); amt = std::stod(field);
        getline(ss, description);

        return Transaction(id, vId, amt, description);
    }
};