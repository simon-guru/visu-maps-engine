/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/commands/command_common.hpp"
#include "engine/gfx/commands/i_command_buffer.hpp"

namespace vme::engine::gfx::commands {

/**
 * @brief Resultado de finalização do encoder.
 */
struct FinishCommandBufferResult {
    std::unique_ptr<ICommandBuffer> command_buffer{};
    CommandError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(command_buffer); }
};

/**
 * @brief Contrato público para gravação de comandos.
 *
 * Decisões suportadas:
 * - encoder é mutável por design durante a gravação;
 * - ordem de chamada define ordem de execução no command buffer final;
 * - ausência de comandos implícitos/mágicos para garantir portabilidade.
 */
class ICommandEncoder {
public:
    virtual ~ICommandEncoder() = default;

    /**
     * @brief Grava operação de cópia.
     */
    virtual CommandError copy(const CopyBufferCommand& command) = 0;

    /**
     * @brief Grava operação de compute dispatch.
     */
    virtual CommandError dispatch(const DispatchCommand& command) = 0;

    /**
     * @brief Grava operação de draw.
     */
    virtual CommandError draw(const DrawCommand& command) = 0;

    /**
     * @brief Grava operação de barreira/sincronização.
     */
    virtual CommandError barrier(const BarrierCommand& command) = 0;

    /**
     * @brief Finaliza gravação e retorna command buffer imutável.
     */
    virtual FinishCommandBufferResult finish(const CommandBufferDesc& desc) = 0;
};

}  // namespace vme::engine::gfx::commands
