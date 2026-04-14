/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/contracts/factory.hpp"

#include <deque>
#include <utility>

namespace vme::engine::gfx::contracts {
namespace {

class StubGfxQueue final : public IGfxQueue {
public:
    QueueSubmitResult submit(const SubmitInfo& submit_info) override {
        const auto validation = validate_submit(submit_info);
        if (!validation.ok()) {
            return validation;
        }

        queue_.push_back(submit_info);
        return QueueSubmitResult::ok_result();
    }

    QueueSubmitResult submit_batch(const SubmitBatch& submit_batch) override {
        for (const auto& submit_info : submit_batch.submissions) {
            const auto validation = validate_submit(submit_info);
            if (!validation.ok()) {
                return validation;
            }
        }

        for (const auto& submit_info : submit_batch.submissions) {
            queue_.push_back(submit_info);
        }

        return QueueSubmitResult::ok_result();
    }

    QueueSubmitResult process_next_submission() override {
        if (queue_.empty()) {
            return {QueueSubmitErrorCode::kInvalidArgument, "queue has no pending submissions"};
        }

        SubmitInfo submission = queue_.front();
        queue_.pop_front();

        for (const auto& signal : submission.signal_semaphores) {
            signal.semaphore->value = signal.value;
        }

        if (submission.signal_fence != nullptr) {
            submission.signal_fence->signaled = true;
        }

        ++completed_submissions_;
        return QueueSubmitResult::ok_result();
    }

    [[nodiscard]] std::uint64_t pending_submission_count() const noexcept override {
        return static_cast<std::uint64_t>(queue_.size());
    }

    [[nodiscard]] std::uint64_t completed_submission_count() const noexcept override {
        return completed_submissions_;
    }

private:
    static QueueSubmitResult validate_submit(const SubmitInfo& submit_info) {
        if (submit_info.command_buffer == nullptr) {
            return {QueueSubmitErrorCode::kInvalidArgument, "submit requires a valid command buffer"};
        }

        for (const auto& wait : submit_info.wait_semaphores) {
            if (wait.semaphore == nullptr) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit wait semaphore reference cannot be null"};
            }

            if (wait.semaphore->value < wait.min_value) {
                return {QueueSubmitErrorCode::kSyncUnresolved,
                        "submit wait semaphore dependency is not satisfied"};
            }
        }

        for (const auto& signal : submit_info.signal_semaphores) {
            if (signal.semaphore == nullptr) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit signal semaphore reference cannot be null"};
            }
        }

        return QueueSubmitResult::ok_result();
    }

    std::deque<SubmitInfo> queue_{};
    std::uint64_t completed_submissions_{0};
};

}  // namespace

std::unique_ptr<IGfxQueue> create_gfx_queue_stub() {
    return std::make_unique<StubGfxQueue>();
}

}  // namespace vme::engine::gfx::contracts
