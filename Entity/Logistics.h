#pragma once
#include <string>
#include <ostream>
#include <sstream>
struct Logistics {
    int logisticsId;
    std::string boothId;
    std::string itemName;
    int quantity;
    double unitCost;
    std::string status;

    Logistics(int id, const std::string& bId, const std::string& item, int qty, double cost)
        : logisticsId(id), boothId(bId), itemName(item), quantity(qty),
        unitCost(cost), status("Requested") {
    }

    void serialize(std::ostream& os) const {
        os << logisticsId << "," << boothId << "," << itemName << "," << quantity << "," << unitCost << "," << status;
    }

    static Logistics deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field, bId, item, stat;
        int id, qty;
        double cost;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, bId, ',');
        getline(ss, item, ',');
        getline(ss, field, ','); qty = std::stoi(field);
        getline(ss, field, ','); cost = std::stod(field);
        getline(ss, stat);

        Logistics log(id, bId, item, qty, cost);
        log.status = stat;
        return log;
    }

    double getTotalCost() const {
        return quantity * unitCost;
    }
};