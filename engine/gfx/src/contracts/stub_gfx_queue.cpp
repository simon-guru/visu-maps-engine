/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/contracts/factory.hpp"

#include <deque>
#include <unordered_map>

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
        std::unordered_map<QueueTimeline*, std::uint64_t> timeline_projection{};
        std::unordered_map<FenceState*, std::uint64_t> fence_projection{};

        for (const auto& submit_info : submit_batch.submissions) {
            const auto validation =
                validate_submit_with_projection(submit_info, timeline_projection, fence_projection);
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

        const SubmitInfo submission = queue_.front();
        queue_.pop_front();

        for (const auto& signal : submission.signals) {
            signal.timeline->value = signal.value;
        }

        if (submission.fence_signal.fence != nullptr) {
            submission.fence_signal.fence->completed_value = submission.fence_signal.value;
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
    static QueueSubmitResult validate_submit_with_projection(
        const SubmitInfo& submit_info,
        std::unordered_map<QueueTimeline*, std::uint64_t>& timeline_projection,
        std::unordered_map<FenceState*, std::uint64_t>& fence_projection) {
        if (submit_info.command_buffer == nullptr) {
            return {QueueSubmitErrorCode::kInvalidArgument, "submit requires a valid command buffer"};
        }

        for (const auto& wait : submit_info.waits) {
            if (wait.timeline == nullptr) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit wait timeline reference cannot be null"};
            }

            const auto projection_it = timeline_projection.find(wait.timeline);
            const auto visible_value =
                projection_it == timeline_projection.end() ? wait.timeline->value : projection_it->second;

            if (visible_value < wait.min_value) {
                return {QueueSubmitErrorCode::kSyncUnresolved,
                        "submit wait timeline dependency is not satisfied"};
            }
        }

        for (const auto& signal : submit_info.signals) {
            if (signal.timeline == nullptr) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit signal timeline reference cannot be null"};
            }

            const auto projection_it = timeline_projection.find(signal.timeline);
            const auto visible_value =
                projection_it == timeline_projection.end() ? signal.timeline->value : projection_it->second;

            if (signal.value < visible_value) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit signal timeline value cannot regress"};
            }

            timeline_projection[signal.timeline] = signal.value;
        }

        if (submit_info.fence_signal.fence != nullptr) {
            const auto projection_it = fence_projection.find(submit_info.fence_signal.fence);
            const auto visible_value = projection_it == fence_projection.end()
                                         ? submit_info.fence_signal.fence->completed_value.value
                                         : projection_it->second;

            if (submit_info.fence_signal.value.value < visible_value) {
                return {QueueSubmitErrorCode::kInvalidArgument,
                        "submit fence value cannot regress"};
            }

            fence_projection[submit_info.fence_signal.fence] = submit_info.fence_signal.value.value;
        }

        return QueueSubmitResult::ok_result();
    }

    static QueueSubmitResult validate_submit(const SubmitInfo& submit_info) {
        std::unordered_map<QueueTimeline*, std::uint64_t> timeline_projection{};
        std::unordered_map<FenceState*, std::uint64_t> fence_projection{};
        return validate_submit_with_projection(submit_info, timeline_projection, fence_projection);
    }

    std::deque<SubmitInfo> queue_{};
    std::uint64_t completed_submissions_{0};
};

class StubGfxDevice final : public IGfxDevice {
public:
    CreateQueueResult create_queue(QueueType /*queue_type*/) override {
        return {create_gfx_queue_stub(), QueueSubmitResult::ok_result()};
    }
};

}  // namespace

std::unique_ptr<IGfxQueue> create_gfx_queue_stub() {
    return std::make_unique<StubGfxQueue>();
}

std::unique_ptr<IGfxDevice> create_gfx_device_stub() {
    return std::make_unique<StubGfxDevice>();
}

}  // namespace vme::engine::gfx::contracts
