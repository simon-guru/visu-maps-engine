/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/gfx/commands/i_command_buffer.hpp"

namespace vme::engine::gfx::contracts {

/**
 * @brief Resultado de submissão de command buffer para fila.
 */
struct QueueSubmitResult {
    bool ok{true};
    const char* message{nullptr};
};

/**
 * @brief Contrato mínimo de fila gráfica para submissão.
 */
class IGfxQueue {
public:
    virtual ~IGfxQueue() = default;

    /**
     * @brief Submete command buffer previamente finalizado.
     */
    virtual QueueSubmitResult submit(const commands::ICommandBuffer& command_buffer) = 0;
};

}  // namespace vme::engine::gfx::contracts
