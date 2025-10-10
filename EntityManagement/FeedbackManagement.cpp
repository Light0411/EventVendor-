#define NOMINMAX
#include "FeedbackManagement.h"
#include "ModuleHelpers.h"
#include "../UniversalFunction/GenericUtils.h"
#include "../Entity/Feedback.h"
#include "../Entity/Vendor.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <memory>
#include <algorithm>

namespace FeedbackManagement {
    static void displayFeedbacksTable(const std::vector<std::unique_ptr<Feedback>>& feedbacks, const std::vector<std::unique_ptr<Vendor>>& vendors) {
        system("cls");
        std::cout << "--- All Feedbacks ---\n\n";
        if (feedbacks.empty()) {
            std::cout << "No feedbacks found.\n";
        }
        else {
            const int id_w = 12, vid_w = 11, vname_w = 20, rating_w = 8, comment_w = 40;
            std::string border = "+" + std::string(id_w, '-') + "+" + std::string(vid_w, '-') + "+" + std::string(vname_w, '-') + "+" + std::string(rating_w, '-') + "+" + std::string(comment_w, '-') + "+\n";
            std::cout << border;
            std::cout << "| " << std::left << std::setw(id_w - 2) << "ID" << " | "
                << std::setw(vid_w - 2) << "Vendor ID" << " | "
                << std::setw(vname_w - 2) << "Vendor Name" << " | "
                << std::setw(rating_w - 2) << "Rating" << " | "
                << std::setw(comment_w - 2) << "Comment" << " |\n";
            std::cout << border;
            for (const auto& f : feedbacks) {
                const Vendor* vendor = GenericUtils::findById(vendors, f->vendorId);
                std::string vendorName = vendor ? vendor->name : "Unknown";
                if (vendorName.length() > vname_w - 2) vendorName = vendorName.substr(0, vname_w - 3) + ".";

                std::string comment = f->comment;
                if (comment.length() > comment_w - 2) comment = comment.substr(0, comment_w - 3) + ".";

                std::cout << "| " << std::left << std::setw(id_w - 2) << f->feedbackId << " | "
                    << std::setw(vid_w - 2) << f->vendorId << " | "
                    << std::setw(vname_w - 2) << vendorName << " | "
                    << std::right << std::setw(rating_w - 2) << f->rating << " | "
                    << std::left << std::setw(comment_w - 2) << comment << " |\n";
            }
            std::cout << border;
        }
        system("pause");
    }

    void manageFeedbacks(std::vector<std::unique_ptr<Feedback>>& feedbacks, const std::vector<std::unique_ptr<Vendor>>& vendors, int& eventHype) {
        int choice = -1;
        while (choice != 0) {
            choice = getSubMenuChoice("Feedbacks");
            switch (choice) {
            case 1: { // Add
                system("cls");
                std::cout << "--- Add New Feedback ---\n";
                int id, vendorId, rating;
                std::string comment;
                std::cout << "Enter Feedback ID: ";
                std::cin >> id;
                if (std::cin.fail() || std::any_of(feedbacks.begin(), feedbacks.end(), [id](const auto& f) { return f->feedbackId == id; })) {
                    std::cout << "Error: Invalid or duplicate ID.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                std::cout << "Enter Vendor ID: ";
                std::cin >> vendorId;
                if (std::cin.fail() || GenericUtils::findById(vendors, vendorId) == nullptr) {
                    std::cout << "Error: Vendor ID not found.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                std::cout << "Enter Rating (1-5): ";
                std::cin >> rating;
                if (std::cin.fail() || rating < 1 || rating > 5) {
                    std::cout << "Error: Invalid rating. Must be between 1 and 5.\n";
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    system("pause");
                    break;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Enter Comment: ";
                getline(std::cin, comment);

                // Create feedback
                auto fb = std::make_unique<Feedback>(id, vendorId, rating, comment);

                // Hype adjustment (+ve for good reviews, -ve for bad)
                eventHype += int((rating - 3) * 10);

                feedbacks.push_back(std::move(fb));
                std::cout << "Feedback added successfully!\n";
                system("pause");
                break;
            }
            case 2: { // View
                displayFeedbacksTable(feedbacks, vendors);
                break;
            }
            case 3: { // Update
                system("cls");
                std::cout << "--- Update Feedback ---\n";
                int id;
                std::cout << "Enter ID of feedback to update: ";
                std::cin >> id;

                Feedback* feedbackToUpdate = nullptr;
                for (const auto& f : feedbacks) {
                    if (f->feedbackId == id) {
                        feedbackToUpdate = f.get();
                        break;
                    }
                }

                if (feedbackToUpdate == nullptr) {
                    std::cout << "Error: Feedback ID not found.\n";
                }
                else {
                    int newRating;
                    std::string newComment;

                    std::cout << "Enter new Rating (1-5) (current: " << feedbackToUpdate->rating << "): ";
                    std::cin >> newRating;
                    if (std::cin.fail() || newRating < 1 || newRating > 5) {
                        std::cout << "Error: Invalid rating. Update cancelled.\n";
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                    else {
                        // adjust hype relative to change
                        eventHype += int((newRating - feedbackToUpdate->rating) * 10);

                        feedbackToUpdate->rating = newRating;

                        std::cout << "Enter new Comment (current: " << feedbackToUpdate->comment << "): ";
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        getline(std::cin, newComment);
                        feedbackToUpdate->comment = newComment;
                        std::cout << "Feedback updated successfully!\n";
                    }
                }
                system("pause");
                break;
            }
            case 4: { // Delete
                system("cls");
                std::cout << "--- Delete Feedback ---\n";
                int id;
                std::cout << "Enter ID of feedback to delete: ";
                std::cin >> id;

                auto it = std::find_if(feedbacks.begin(), feedbacks.end(), [id](const auto& f) {
                    return f->feedbackId == id;
                    });

                if (it == feedbacks.end()) {
                    std::cout << "Error: Feedback not found.\n";
                }
                else {
                    // subtract hype contribution before erasing
                    eventHype -= int(((*it)->rating - 3) * 10);

                    feedbacks.erase(it);
                    std::cout << "Feedback deleted successfully.\n";
                }
                system("pause");
                break;
            }
            case 0: return;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
            }
        }
    }
}
