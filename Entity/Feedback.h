#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Feedback {
    int feedbackId;
    int vendorId;
    int rating; // e.g., 1-5
    std::string comment;

    Feedback(int id, int vId, int r, const std::string& c)
        : feedbackId(id), vendorId(vId), rating(r), comment(c) {
    }

    void serialize(std::ostream& os) const {
        os << feedbackId << "," << vendorId << "," << rating << "," << comment;
    }

    static Feedback deserialize(const std::string& line) {
        std::stringstream ss(line);
        std::string field, comment;
        int id, vId, rating;

        getline(ss, field, ','); id = std::stoi(field);
        getline(ss, field, ','); vId = std::stoi(field);
        getline(ss, field, ','); rating = std::stoi(field);
        getline(ss, comment);

        return Feedback(id, vId, rating, comment);
    }
};