#include "DatabaseManager.h"
#include "../sqlite3.h" // Include the SQLite header
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower, std::ispunct

// Include all entity headers
#include "../Entity/Vendor.h"
#include "../Entity/Booth.h"
#include "../Entity/Product.h"
#include "../Entity/Transaction.h"
#include "../Entity/Logistics.h"
#include "../Entity/Booking.h"
#include "../Entity/Sponsorship.h"
#include "../Entity/MarketingCampaign.h"
#include "../Entity/Feedback.h"

// Helper function to execute simple SQL commands and check for errors
static void executeSQL(sqlite3* db, const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

// Constructor: Opens the database connection and creates tables if needed
DatabaseManager::DatabaseManager(const std::string& dbFilePath) {
    if (sqlite3_open(dbFilePath.c_str(), &m_db_connection) == SQLITE_OK) {
        std::cout << "Successfully connected to SQLite database: " << dbFilePath << std::endl;
        createTables(); // Ensure all tables exist
    }
    else {
        std::cerr << "Failed to open SQLite database: " << sqlite3_errmsg(m_db_connection) << std::endl;
        m_db_connection = nullptr;
    }
}

// Destructor: Closes the database connection
DatabaseManager::~DatabaseManager() {
    if (m_db_connection) {
        sqlite3_close(m_db_connection);
        std::cout << "SQLite database connection closed." << std::endl;
    }
}

// Creates all necessary tables for the application
void DatabaseManager::createTables() {
    if (!m_db_connection) return;
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS vendors (vendorId INTEGER PRIMARY KEY, name TEXT, email TEXT, isFeatured INTEGER, registeredEvents TEXT);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS booths (boothId TEXT PRIMARY KEY, mallName TEXT, floorLevel INTEGER, type TEXT, price REAL, assignedVendorId INTEGER, row INTEGER, col INTEGER);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS products (productId INTEGER PRIMARY KEY, vendorId INTEGER, name TEXT, price REAL);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS transactions (transactionId INTEGER PRIMARY KEY, vendorId INTEGER, amount REAL, description TEXT);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS logistics (logisticsId INTEGER PRIMARY KEY, boothId TEXT, itemName TEXT, quantity INTEGER, unitCost REAL, status TEXT);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS bookings (bookingId INTEGER PRIMARY KEY, boothId TEXT, vendorId INTEGER, startDate TEXT, endDate TEXT, status TEXT, contractTerms TEXT, regulations TEXT);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS sponsorships (sponsorId INTEGER PRIMARY KEY, name TEXT, amount REAL, description TEXT);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS marketing_campaigns (campaignId INTEGER PRIMARY KEY, campaignType TEXT, name TEXT, description TEXT, costToEvent REAL, hypePoints INTEGER, status TEXT, packagePrice REAL, targetVendorId INTEGER, targetProductId INTEGER, colorCode INTEGER);");
    executeSQL(m_db_connection, "CREATE TABLE IF NOT EXISTS feedbacks (feedbackId INTEGER PRIMARY KEY, vendorId INTEGER, rating INTEGER, comment TEXT);");
}

// Loads all data from the database into memory
void DatabaseManager::loadAllData(
    std::vector<std::unique_ptr<Vendor>>& vendors, std::vector<std::unique_ptr<Booth>>& booths,
    std::vector<std::unique_ptr<Product>>& products, std::vector<std::unique_ptr<Transaction>>& transactions,
    std::vector<std::unique_ptr<Logistics>>& logistics, std::vector<std::unique_ptr<Booking>>& bookings,
    std::vector<std::unique_ptr<Sponsorship>>& sponsorships, std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
    std::vector<std::unique_ptr<Feedback>>& feedbacks
) {
    if (!m_db_connection) return;
    sqlite3_stmt* stmt;

    // Load Vendors
    vendors.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT vendorId, name, email, isFeatured, registeredEvents FROM vendors ORDER BY vendorId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto v = std::make_unique<Vendor>(sqlite3_column_int(stmt, 0), (const char*)sqlite3_column_text(stmt, 1), (const char*)sqlite3_column_text(stmt, 2));
            v->isFeatured = sqlite3_column_int(stmt, 3);
            const char* eventsStr = (const char*)sqlite3_column_text(stmt, 4);
            if (eventsStr) {
                std::stringstream ss(eventsStr);
                std::string event;
                while (std::getline(ss, event, ';')) { if (!event.empty()) v->registeredEvents.push_back(event); }
            }
            vendors.push_back(std::move(v));
        }
    }
    sqlite3_finalize(stmt);

    // Load Booths
    booths.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT boothId, mallName, floorLevel, type, price, assignedVendorId, row, col FROM booths ORDER BY boothId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto b = std::make_unique<Booth>((const char*)sqlite3_column_text(stmt, 0), (const char*)sqlite3_column_text(stmt, 1), sqlite3_column_int(stmt, 2), (const char*)sqlite3_column_text(stmt, 3), sqlite3_column_double(stmt, 4));
            b->assignedVendorId = sqlite3_column_int(stmt, 5);
            b->row = sqlite3_column_int(stmt, 6);
            b->col = sqlite3_column_int(stmt, 7);
            booths.push_back(std::move(b));
        }
    }
    sqlite3_finalize(stmt);

    // Load Products
    products.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT productId, vendorId, name, price FROM products ORDER BY productId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            products.push_back(std::make_unique<Product>(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), (const char*)sqlite3_column_text(stmt, 2), sqlite3_column_double(stmt, 3)));
        }
    }
    sqlite3_finalize(stmt);

    // Load Transactions
    transactions.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT transactionId, vendorId, amount, description FROM transactions ORDER BY transactionId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            transactions.push_back(std::make_unique<Transaction>(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_double(stmt, 2), (const char*)sqlite3_column_text(stmt, 3)));
        }
    }
    sqlite3_finalize(stmt);

    // Load Bookings
    bookings.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT bookingId, boothId, vendorId, startDate, endDate, status, contractTerms, regulations FROM bookings ORDER BY bookingId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto b = std::make_unique<Booking>(sqlite3_column_int(stmt, 0), (const char*)sqlite3_column_text(stmt, 1), sqlite3_column_int(stmt, 2), (const char*)sqlite3_column_text(stmt, 3), (const char*)sqlite3_column_text(stmt, 4));
            b->status = (const char*)sqlite3_column_text(stmt, 5);
            const char* terms = (const char*)sqlite3_column_text(stmt, 6);
            if (terms) b->contractTerms = terms;
            const char* regs = (const char*)sqlite3_column_text(stmt, 7);
            if (regs) b->regulations = regs;
            bookings.push_back(std::move(b));
        }
    }
    sqlite3_finalize(stmt);

    // Load Logistics
    logistics.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT logisticsId, boothId, itemName, quantity, unitCost, status FROM logistics ORDER BY logisticsId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto l = std::make_unique<Logistics>(sqlite3_column_int(stmt, 0), (const char*)sqlite3_column_text(stmt, 1), (const char*)sqlite3_column_text(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_double(stmt, 4));
            l->status = (const char*)sqlite3_column_text(stmt, 5);
            logistics.push_back(std::move(l));
        }
    }
    sqlite3_finalize(stmt);

    // Load Sponsorships
    sponsorships.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT sponsorId, name, amount, description FROM sponsorships ORDER BY sponsorId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            sponsorships.push_back(std::make_unique<Sponsorship>(sqlite3_column_int(stmt, 0), (const char*)sqlite3_column_text(stmt, 1), sqlite3_column_double(stmt, 2), (const char*)sqlite3_column_text(stmt, 3)));
        }
    }
    sqlite3_finalize(stmt);

    // Load Campaigns
    campaigns.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT campaignId, campaignType, name, description, costToEvent, hypePoints, status, packagePrice, targetVendorId, targetProductId, colorCode FROM marketing_campaigns ORDER BY campaignId", -1, &stmt, 0) == SQLITE_OK) {

        // ============================ FIX START ============================
        // Define the hardcoded ASCII art templates here
        std::vector<std::string> socialArt = { R"(    .--.     )", R"(   |o_o |    )", R"(   |\_/ |    )", R"(  //   \ \   )", R"( (|     | )  )", R"('/\_   _/`\ )", R"(\___)=(___/)" };
        std::vector<std::string> flyerArt = { R"(+----------------+)", R"(|   COME TO THE  |)", R"(| ## EVENT ##   |)", R"(|  [Date Here]   |)", R"(+----------------+)" };
        std::vector<std::string> bannerArt = { R"(+================+)", R"(|  PROMOTION!    |)", R"(|  Check out our |)", R"(|  amazing deal! |)", R"(+================+)" };
        std::vector<std::string> tvArt = { R"(.---------------.)", R"(|[  ___  ] [ TV ]|)", R"(| [_|_]   [___] |)", R"(|_______________|)", R"(\_/)__|_|__(\_/)" };

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string type = (const char*)sqlite3_column_text(stmt, 1);
            std::vector<std::string> art; // Vector to hold the chosen art

            // Select the correct art based on the campaign type from the database
            if (type == "Social Media Campaign") art = socialArt;
            else if (type == "Street Team Promoters") art = flyerArt;
            else if (type == "Online Banner Ads") art = bannerArt;
            else if (type == "TV Commercial Slot") art = tvArt;

            MarketingCampaign::PackageInfo pkg(sqlite3_column_double(stmt, 7), true);

            // Create the campaign object, passing the selected `art` vector
            auto c = std::make_unique<MarketingCampaign>(
                sqlite3_column_int(stmt, 0),
                type,
                (const char*)sqlite3_column_text(stmt, 2),
                (const char*)sqlite3_column_text(stmt, 3),
                sqlite3_column_int(stmt, 8),
                sqlite3_column_int(stmt, 9),
                sqlite3_column_int(stmt, 5),
                sqlite3_column_int(stmt, 10),
                art, // Pass the correct art here
                pkg
            );
            // ============================= FIX END =============================

            c->costToEvent = sqlite3_column_double(stmt, 4);
            c->status = (const char*)sqlite3_column_text(stmt, 6);
            campaigns.push_back(std::move(c));
        }
    }
    sqlite3_finalize(stmt);

    // Load Feedbacks
    feedbacks.clear();
    if (sqlite3_prepare_v2(m_db_connection, "SELECT feedbackId, vendorId, rating, comment FROM feedbacks ORDER BY feedbackId", -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            feedbacks.push_back(std::make_unique<Feedback>(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2), (const char*)sqlite3_column_text(stmt, 3)));
        }
    }
    sqlite3_finalize(stmt);

    std::cout << "All data loaded from SQLite database." << std::endl;
}

// Saves all data from memory back into the database
void DatabaseManager::saveAllData(
    const std::vector<std::unique_ptr<Vendor>>& vendors, const std::vector<std::unique_ptr<Booth>>& booths,
    const std::vector<std::unique_ptr<Product>>& products, const std::vector<std::unique_ptr<Transaction>>& transactions,
    const std::vector<std::unique_ptr<Logistics>>& logistics, const std::vector<std::unique_ptr<Booking>>& bookings,
    const std::vector<std::unique_ptr<Sponsorship>>& sponsorships, const std::vector<std::unique_ptr<MarketingCampaign>>& campaigns,
    const std::vector<std::unique_ptr<Feedback>>& feedbacks
) {
    if (!m_db_connection) return;
    executeSQL(m_db_connection, "BEGIN TRANSACTION;");

    // Clear all tables to prevent duplicates
    executeSQL(m_db_connection, "DELETE FROM vendors;");
    executeSQL(m_db_connection, "DELETE FROM booths;");
    executeSQL(m_db_connection, "DELETE FROM products;");
    executeSQL(m_db_connection, "DELETE FROM transactions;");
    executeSQL(m_db_connection, "DELETE FROM logistics;");
    executeSQL(m_db_connection, "DELETE FROM bookings;");
    executeSQL(m_db_connection, "DELETE FROM sponsorships;");
    executeSQL(m_db_connection, "DELETE FROM marketing_campaigns;");
    executeSQL(m_db_connection, "DELETE FROM feedbacks;");

    sqlite3_stmt* stmt;

    // Save Vendors
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO vendors (vendorId, name, email, isFeatured, registeredEvents) VALUES (?, ?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& v : vendors) {
            std::string eventsStr;
            for (size_t i = 0; i < v->registeredEvents.size(); ++i) { eventsStr += v->registeredEvents[i] + (i < v->registeredEvents.size() - 1 ? ";" : ""); }
            sqlite3_bind_int(stmt, 1, v->vendorId);
            sqlite3_bind_text(stmt, 2, v->name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, v->email.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 4, v->isFeatured);
            sqlite3_bind_text(stmt, 5, eventsStr.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving vendor\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Booths
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO booths (boothId, mallName, floorLevel, type, price, assignedVendorId, row, col) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& b : booths) {
            sqlite3_bind_text(stmt, 1, b->boothId.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, b->mallName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 3, b->floorLevel);
            sqlite3_bind_text(stmt, 4, b->type.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 5, b->price);
            sqlite3_bind_int(stmt, 6, b->assignedVendorId);
            sqlite3_bind_int(stmt, 7, b->row);
            sqlite3_bind_int(stmt, 8, b->col);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving booth\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Products
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO products (productId, vendorId, name, price) VALUES (?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& p : products) {
            sqlite3_bind_int(stmt, 1, p->productId);
            sqlite3_bind_int(stmt, 2, p->vendorId);
            sqlite3_bind_text(stmt, 3, p->name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 4, p->price);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving product\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Transactions
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO transactions (transactionId, vendorId, amount, description) VALUES (?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& t : transactions) {
            sqlite3_bind_int(stmt, 1, t->transactionId);
            sqlite3_bind_int(stmt, 2, t->vendorId);
            sqlite3_bind_double(stmt, 3, t->amount);
            sqlite3_bind_text(stmt, 4, t->description.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving transaction\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Bookings
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO bookings (bookingId, boothId, vendorId, startDate, endDate, status, contractTerms, regulations) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& b : bookings) {
            sqlite3_bind_int(stmt, 1, b->bookingId);
            sqlite3_bind_text(stmt, 2, b->boothId.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 3, b->vendorId);
            sqlite3_bind_text(stmt, 4, b->startDate.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, b->endDate.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 6, b->status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 7, b->contractTerms.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 8, b->regulations.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving booking\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Logistics
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO logistics (logisticsId, boothId, itemName, quantity, unitCost, status) VALUES (?, ?, ?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& l : logistics) {
            sqlite3_bind_int(stmt, 1, l->logisticsId);
            sqlite3_bind_text(stmt, 2, l->boothId.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, l->itemName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 4, l->quantity);
            sqlite3_bind_double(stmt, 5, l->unitCost);
            sqlite3_bind_text(stmt, 6, l->status.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving logistics item\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Sponsorships
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO sponsorships (sponsorId, name, amount, description) VALUES (?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& s : sponsorships) {
            sqlite3_bind_int(stmt, 1, s->sponsorId);
            sqlite3_bind_text(stmt, 2, s->name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 3, s->amount);
            sqlite3_bind_text(stmt, 4, s->description.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving sponsorship\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Campaigns
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO marketing_campaigns (campaignId, campaignType, name, description, costToEvent, hypePoints, status, packagePrice, targetVendorId, targetProductId, colorCode) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& c : campaigns) {
            sqlite3_bind_int(stmt, 1, c->campaignId);
            sqlite3_bind_text(stmt, 2, c->campaignType.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, c->name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, c->description.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 5, c->costToEvent);
            sqlite3_bind_int(stmt, 6, c->hypePoints);
            sqlite3_bind_text(stmt, 7, c->status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 8, c->package.priceForVendor);
            sqlite3_bind_int(stmt, 9, c->targetVendorId);
            sqlite3_bind_int(stmt, 10, c->targetProductId);
            sqlite3_bind_int(stmt, 11, c->colorCode);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error saving campaign\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Save Feedbacks
    if (sqlite3_prepare_v2(m_db_connection, "INSERT INTO feedbacks (feedbackId, vendorId, rating, comment) VALUES (?, ?, ?, ?)", -1, &stmt, 0) == SQLITE_OK) {
        for (const auto& f : feedbacks) {
            sqlite3_bind_int(stmt, 1, f->feedbackId);
            sqlite3_bind_int(stmt, 2, f->vendorId);
            sqlite3_bind_int(stmt, 3, f->rating);
            sqlite3_bind_text(stmt, 4, f->comment.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) != SQLITE_DONE) std::cerr << "Error inserting feedback\n";
            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    executeSQL(m_db_connection, "COMMIT;");
    std::cout << "All data saved successfully to SQLite database." << std::endl;
}