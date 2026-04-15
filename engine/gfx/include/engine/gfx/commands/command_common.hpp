/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>

namespace vme::engine::gfx::commands {

/**
 * @brief Código de erro da camada de comandos.
 *
 * Decisão de design:
 * - enum fechado para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 */
enum class CommandErrorCode : std::uint16_t {
    kOk = 0,
    kInvalidState,
    kInvalidArgument,
    kCapabilityNotSupported,
    kOutOfMemory,
    kBackendFailure,
};

/**
 * @brief Resultado padrão para operações de gravação/finalização de comandos.
 */
struct CommandError {
    CommandErrorCode code{CommandErrorCode::kOk};
    std::string message{};

    [[nodiscard]] bool ok() const noexcept { return code == CommandErrorCode::kOk; }

    static CommandError ok_result() noexcept { return {}; }
};

/**
 * @brief Metadados declarativos de um command buffer.
 */
struct CommandBufferDesc {
    std::string label{};
    bool one_time_submit{false};
};

/**
 * @brief Operação de cópia linear entre buffers.
 */
struct CopyBufferCommand {
    std::uint64_t src_offset{0};
    std::uint64_t dst_offset{0};
    std::uint64_t size_bytes{0};
};

/**
 * @brief Operação de dispatch para workloads de compute.
 */
struct DispatchCommand {
    std::uint32_t group_count_x{1};
    std::uint32_t group_count_y{1};
    std::uint32_t group_count_z{1};
};

/**
 * @brief Operação de draw para workloads gráficos.
 */
struct DrawCommand {
    std::uint32_t vertex_count{0};
    std::uint32_t instance_count{1};
    std::uint32_t first_vertex{0};
    std::uint32_t first_instance{0};
};

/**
 * @brief Escopo de estágios abstratos para sincronização.
 */
enum class PipelineStage : std::uint8_t {
    kTop,
    kTransfer,
    kVertex,
    kFragment,
    kCompute,
    kBottom,
};

/**
 * @brief Operação de barreira abstrata entre estágios.
 */
struct BarrierCommand {
    PipelineStage src_stage{PipelineStage::kTop};
    PipelineStage dst_stage{PipelineStage::kBottom};
};

}  // namespace vme::engine::gfx::commands
