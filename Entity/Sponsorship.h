#pragma once
#include <string>
#include <ostream>
#include <sstream>

struct Sponsorship {
    int sponsorId;
    std::string name;
    double amount;
    std::string description; 

    Sponsorship(int id, const std::string& n, double amt, const std::string& desc)
        : sponsorId(id), name(n), amount(amt), description(desc) {
    }
};