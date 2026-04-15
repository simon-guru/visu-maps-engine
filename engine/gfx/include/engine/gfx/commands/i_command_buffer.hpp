/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

#include "engine/gfx/commands/command_common.hpp"

namespace vme::engine::gfx::commands {

/**
 * @brief Contrato imutável de comandos finalizados.
 *
 * Justificativa:
 * - separa fase de construção (encoder mutável) da fase de execução;
 * - reduz efeitos colaterais e facilita rastreio determinístico.
 */
class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;

    /**
     * @brief Descritor usado na finalização do buffer.
     */
    [[nodiscard]] virtual const CommandBufferDesc& desc() const noexcept = 0;

    /**
     * @brief Quantidade total de comandos gravados.
     */
    [[nodiscard]] virtual std::uint64_t command_count() const noexcept = 0;
};

}  // namespace vme::engine::gfx::commands
