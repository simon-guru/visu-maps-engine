/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/contracts/factory.hpp"

#include <vector>

namespace vme::engine::gfx::contracts {
namespace {

class StubGfxSwapchain final : public IGfxSwapchain {
public:
    explicit StubGfxSwapchain(const SwapchainDesc& desc) : image_count_(desc.image_count) {}

    AcquireImageResult acquire_next_image() override {
        if (image_count_ == 0) {
            return {SwapchainErrorCode::kInvalidState, 0};
        }
        if (acquired_) {
            return {SwapchainErrorCode::kInvalidState, 0};
        }

        acquired_ = true;
        current_image_index_ = next_image_index_;
        next_image_index_ = (next_image_index_ + 1) % image_count_;
        return {SwapchainErrorCode::kOk, current_image_index_};
    }

    PresentResult present(std::uint32_t image_index) override {
        if (!acquired_ || image_index != current_image_index_) {
            return {SwapchainErrorCode::kInvalidState};
        }

        acquired_ = false;
        return {SwapchainErrorCode::kOk};
    }

    SwapchainErrorCode recreate(const SwapchainDesc& desc) override {
        if (desc.image_count < 2) {
            return SwapchainErrorCode::kInvalidArgument;
        }

        image_count_ = desc.image_count;
        next_image_index_ = 0;
        current_image_index_ = 0;
        acquired_ = false;
        return SwapchainErrorCode::kOk;
    }

private:
    std::uint32_t image_count_{2};
    std::uint32_t next_image_index_{0};
    std::uint32_t current_image_index_{0};
    bool acquired_{false};
};

class StubGfxDevice final : public IGfxDevice {
public:
    CreateQueueResult create_queue(QueueType /*queue_type*/) override {
        return {create_gfx_queue_stub(), QueueSubmitResult::ok_result()};
    }

    CreateSwapchainResult create_swapchain(const SwapchainDesc& desc) override {
        if (desc.image_count < 2) {
            return {nullptr, SwapchainErrorCode::kInvalidArgument};
        }

        return {std::make_unique<StubGfxSwapchain>(desc), SwapchainErrorCode::kOk};
    }
};

class StubGfxInstance final : public IGfxInstance {
public:
    StubGfxInstance() { adapters_.push_back(AdapterInfo {.adapter_id = 0, .name = "stub-default-adapter"}); }

    [[nodiscard]] std::uint32_t adapter_count() const noexcept override {
        return static_cast<std::uint32_t>(adapters_.size());
    }

    [[nodiscard]] AdapterInfo adapter_info(std::uint32_t adapter_index) const noexcept override {
        if (adapter_index >= adapters_.size()) {
            return AdapterInfo {.adapter_id = 0, .name = "invalid-adapter"};
        }

        return adapters_[adapter_index];
    }

    CreateDeviceResult create_device(std::uint32_t adapter_index) override {
        if (adapter_index >= adapters_.size()) {
            return {nullptr, {QueueSubmitErrorCode::kInvalidArgument, "adapter index out of range"}};
        }

        return {std::make_unique<StubGfxDevice>(), QueueSubmitResult::ok_result()};
    }

private:
    std::vector<AdapterInfo> adapters_{};
};

}  // namespace

std::unique_ptr<IGfxDevice> create_gfx_device_stub() {
    return std::make_unique<StubGfxDevice>();
}

std::unique_ptr<IGfxInstance> create_gfx_instance_stub() {
    return std::make_unique<StubGfxInstance>();
}

}  // namespace vme::engine::gfx::contracts
