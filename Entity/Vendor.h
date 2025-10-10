#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Vendor {
    int vendorId;
    std::string name;
    std::string email;
    bool isFeatured = false;
    std::vector<std::string> registeredEvents;

    Vendor(int id, const std::string& n, const std::string& e)
        : vendorId(id), name(n), email(e), isFeatured(false) {
    }

    void serialize(std::ostream& os) const {
        os << vendorId << "," << name << "," << email;
    }

    // Static method to create a Vendor by reading a line of text
    static Vendor deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field;
        int id;
        std::string name, email;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, name, ',');
        getline(ss, email);

        return Vendor(id, name, email);
    }
};