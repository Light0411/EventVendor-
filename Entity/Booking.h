#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Booking {
    int bookingId;
    std::string boothId;
    int vendorId;
    std::string startDate;
    std::string endDate;
    std::string status;
    std::string contractTerms; 
    std::string regulations;    

    Booking(int id, const std::string& bId, int vId, const std::string& start, const std::string& end)
        : bookingId(id), boothId(bId), vendorId(vId), startDate(start), endDate(end), status("Confirmed") {
    }

    void serialize(std::ostream& os) const {
        os << bookingId << "," << boothId << "," << vendorId << "," << startDate << "," << endDate << "," << status;
    }

    static Booking deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field, bId, start, end, stat;
        int id, vId;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, bId, ',');
        getline(ss, field, ','); vId = std::stoi(field);
        getline(ss, start, ',');
        getline(ss, end, ',');
        getline(ss, stat);

        Booking booking(id, bId, vId, start, end);
        booking.status = stat;
        return booking;
    }
};