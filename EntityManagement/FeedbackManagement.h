#pragma once
#include <vector>
#include <memory>

struct Feedback;
struct Vendor;

namespace FeedbackManagement {
    void manageFeedbacks(
        std::vector<std::unique_ptr<Feedback>>& feedbacks,
		const std::vector<std::unique_ptr<Vendor>>& vendors, int& eventHype
    );
}