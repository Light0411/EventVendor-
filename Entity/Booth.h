#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Booth {
    std::string boothId;
    std::string mallName;
    int floorLevel;
    std::string type;
    double price;
    int assignedVendorId = 0;
    int row = -1;
    int col = -1;

    Booth(const std::string& id, const std::string& mall, int floor, const std::string& t, double p)
        : boothId(id), mallName(mall), floorLevel(floor), type(t), price(p),
        assignedVendorId(0), row(-1), col(-1) {
    }

    void serialize(std::ostream& os) const {
        os << boothId << "," << mallName << "," << floorLevel << "," << type << "," << price << "," << assignedVendorId << "," << row << "," << col;
    }

    static Booth deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string id, mall, type, field;
        int floor, assignedId, r, c;
        double p;

        getline(ss, id, ',');
        getline(ss, mall, ',');
        getline(ss, field, ','); floor = std::stoi(field);
        getline(ss, type, ',');
        getline(ss, field, ','); p = std::stod(field);
        getline(ss, field, ','); assignedId = std::stoi(field);
        getline(ss, field, ','); r = std::stoi(field);
        getline(ss, field, ','); c = std::stoi(field);

        Booth booth(id, mall, floor, type, p);
        booth.assignedVendorId = assignedId;
        booth.row = r;
        booth.col = c;
        return booth;
    }
};