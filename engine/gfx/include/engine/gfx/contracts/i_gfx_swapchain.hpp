/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

namespace vme::engine::gfx::contracts {

struct SwapchainDesc {
    std::uint32_t image_count{2};
};

enum class SwapchainErrorCode : std::uint16_t {
    kOk = 0,
    kInvalidArgument,
    kInvalidState,
    kOutOfDate,
};

struct AcquireImageResult {
    SwapchainErrorCode code{SwapchainErrorCode::kOk};
    std::uint32_t image_index{0};

    [[nodiscard]] bool ok() const noexcept { return code == SwapchainErrorCode::kOk; }
};

struct PresentResult {
    SwapchainErrorCode code{SwapchainErrorCode::kOk};

    [[nodiscard]] bool ok() const noexcept { return code == SwapchainErrorCode::kOk; }
};

class IGfxSwapchain {
public:
    virtual ~IGfxSwapchain() = default;

    virtual AcquireImageResult acquire_next_image() = 0;
    virtual PresentResult present(std::uint32_t image_index) = 0;
    virtual SwapchainErrorCode recreate(const SwapchainDesc& desc) = 0;
};

}  // namespace vme::engine::gfx::contracts
