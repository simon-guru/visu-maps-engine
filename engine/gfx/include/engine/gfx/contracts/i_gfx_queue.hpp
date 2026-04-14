/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <vector>

#include "engine/gfx/commands/i_command_buffer.hpp"

namespace vme::engine::gfx::contracts {

/**
 * @brief Valor monotônico de timeline para sincronização incremental.
 */
struct QueueTimeline {
    std::uint64_t value{0};
};

/**
 * @brief Valor explícito de fence usado para sinalização por submissão.
 */
struct FenceValue {
    std::uint64_t value{0};
};

/**
 * @brief Estado de fence com último valor concluído pela fila.
 */
struct FenceState {
    FenceValue completed_value{};
};

/**
 * @brief Dependência de espera em timeline.
 */
struct TimelineWaitInfo {
    QueueTimeline* timeline{nullptr};
    std::uint64_t min_value{1};
};

/**
 * @brief Sinalização de timeline após execução da submissão.
 */
struct TimelineSignalInfo {
    QueueTimeline* timeline{nullptr};
    std::uint64_t value{1};
};

/**
 * @brief Sinalização de fence após execução da submissão.
 */
struct FenceSignalInfo {
    FenceState* fence{nullptr};
    FenceValue value{1};
};

/**
 * @brief Informações de uma única submissão para fila.
 */
struct SubmitInfo {
    const commands::ICommandBuffer* command_buffer{nullptr};
    std::vector<TimelineWaitInfo> waits{};
    std::vector<TimelineSignalInfo> signals{};
    FenceSignalInfo fence_signal{};
};

/**
 * @brief Lote de submissões processadas em ordem estável de inserção.
 */
struct SubmitBatch {
    std::vector<SubmitInfo> submissions{};
};

enum class QueueSubmitErrorCode : std::uint16_t {
    kOk = 0,
    kInvalidArgument,
    kSyncUnresolved,
    kInternalFailure,
};

/**
 * @brief Resultado de submissão/processamento de fila.
 */
struct QueueSubmitResult {
    QueueSubmitErrorCode code{QueueSubmitErrorCode::kOk};
    const char* message{nullptr};

    [[nodiscard]] bool ok() const noexcept { return code == QueueSubmitErrorCode::kOk; }

    static QueueSubmitResult ok_result() noexcept { return {}; }
};

/**
 * @brief Contrato de fila gráfica para submissão e processamento.
 */
class IGfxQueue {
public:
    virtual ~IGfxQueue() = default;

    /**
     * @brief Agenda uma submissão para execução assíncrona na fila.
     */
    virtual QueueSubmitResult submit(const SubmitInfo& submit_info) = 0;

    /**
     * @brief Agenda múltiplas submissões respeitando ordem estável do lote.
     */
    virtual QueueSubmitResult submit_batch(const SubmitBatch& submit_batch) = 0;

    /**
     * @brief Processa próxima submissão pendente e aplica sinalizações.
     */
    virtual QueueSubmitResult process_next_submission() = 0;

    /**
     * @brief Quantidade de submissões pendentes de execução.
     */
    [[nodiscard]] virtual std::uint64_t pending_submission_count() const noexcept = 0;

    /**
     * @brief Quantidade de submissões já processadas pela fila.
     */
    [[nodiscard]] virtual std::uint64_t completed_submission_count() const noexcept = 0;
};

}  // namespace vme::engine::gfx::contracts
