/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/contracts/i_gfx_queue.hpp"
#include "engine/gfx/contracts/i_gfx_swapchain.hpp"

namespace vme::engine::gfx::contracts {

enum class QueueType {
    kGraphics,
    kCompute,
    kTransfer,
};

struct CreateQueueResult {
    std::unique_ptr<IGfxQueue> queue{};
    QueueSubmitResult error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(queue); }
};

struct CreateSwapchainResult {
    std::unique_ptr<IGfxSwapchain> swapchain{};
    SwapchainErrorCode error{SwapchainErrorCode::kOk};

    [[nodiscard]] bool ok() const noexcept {
        return error == SwapchainErrorCode::kOk && static_cast<bool>(swapchain);
    }
};

/**
 * @brief Contrato mínimo de dispositivo para criação de filas e swapchain.
 */
class IGfxDevice {
public:
    virtual ~IGfxDevice() = default;

    virtual CreateQueueResult create_queue(QueueType queue_type) = 0;
    virtual CreateSwapchainResult create_swapchain(const SwapchainDesc& desc) = 0;
};

}  // namespace vme::engine::gfx::contracts
