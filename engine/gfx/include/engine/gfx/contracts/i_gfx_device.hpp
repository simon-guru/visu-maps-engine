/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/contracts/i_gfx_queue.hpp"

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

/**
 * @brief Contrato mínimo de dispositivo para criação de filas.
 */
class IGfxDevice {
public:
    virtual ~IGfxDevice() = default;

    /**
     * @brief Cria fila do tipo solicitado.
     */
    virtual CreateQueueResult create_queue(QueueType queue_type) = 0;
};

}  // namespace vme::engine::gfx::contracts
