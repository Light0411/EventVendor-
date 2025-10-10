#pragma once
#include <vector>
#include <memory>
#include <string>

// Forward declare sqlite3 structure to avoid including the full header here
struct sqlite3;

// Forward declare all entity structs
struct Vendor;
struct Booth;
struct Product;
struct Transaction;
struct Logistics;
struct Booking;
struct Sponsorship;
struct MarketingCampaign;
struct Feedback;

class DatabaseManager {
public:
    // Constructor takes the path to the SQLite database file
    DatabaseManager(const std::string& dbFilePath);
    // Destructor to close the database connection
    ~DatabaseManager();

    // Loads all data from the database into the provided vectors
    void loadAllData(
        std::vector<std::unique_ptr<Vendor>>& vendors,
        std::vector<std::unique_ptr<Booth>>& booths,
        std::vector<std::unique_ptr<Product>>& products,
        std::vector<std::unique_ptr<Transaction>>& transactions,
        std::vector<std::unique_ptr<Logistics>>& logistics,
        std::vector<std::unique_ptr<Booking>>& bookings,
        std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
        std::vector<std::unique_ptr<Feedback>>& feedbacks
    );

    // Saves all data from the vectors back into the database
    void saveAllData(
        const std::vector<std::unique_ptr<Vendor>>& vendors,
        const std::vector<std::unique_ptr<Booth>>& booths,
        const std::vector<std::unique_ptr<Product>>& products,
        const std::vector<std::unique_ptr<Transaction>>& transactions,
        const std::vector<std::unique_ptr<Logistics>>& logistics,
        const std::vector<std::unique_ptr<Booking>>& bookings,
        const std::vector<std::unique_ptr<Sponsorship>>& sponsorships,
        const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
        const std::vector<std::unique_ptr<Feedback>>& feedbacks
    );

private:
    // Helper function to create tables if they don't exist
    void createTables();
    // Pointer to the active SQLite database connection
    sqlite3* m_db_connection = nullptr;
};