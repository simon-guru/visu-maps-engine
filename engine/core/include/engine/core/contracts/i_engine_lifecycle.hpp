#pragma once

#include "engine/core/types/engine_models.hpp"

namespace visu::engine::core::contracts {

/**
 * @brief Contrato mínimo para lifecycle do runtime da engine.
 *
 * Inicializa recursos, executa ticks de frame e conduz shutdown seguro.
 */
class IEngineLifecycle {
public:
    virtual ~IEngineLifecycle() = default;

    /**
     * @brief Realiza bootstrap da engine.
     */
    virtual types::EngineError initialize(const types::EngineConfig& config) = 0;

    /**
     * @brief Processa um frame da engine com contexto temporal.
     */
    virtual types::EngineError tick(const types::FrameContext& frame_context) = 0;

    /**
     * @brief Encerra runtime e libera recursos.
     */
    virtual types::EngineError shutdown() = 0;

    /**
     * @brief Retorna o estado atual do lifecycle.
     */
    [[nodiscard]] virtual types::EngineState state() const noexcept = 0;
};

}  // namespace visu::engine::core::contracts
