/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato base para lifecycle do runtime da engine.
 *
 * Nesta revisão, o contrato foi expandido para cobrir também pausa/retomada
 * e leitura da configuração efetivamente carregada.
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
     * @brief Pausa a execução do runtime sem desalocar recursos.
     */
    virtual types::EngineError pause() = 0;

    /**
     * @brief Retoma a execução após estado de pausa.
     */
    virtual types::EngineError resume() = 0;

    /**
     * @brief Encerra runtime e libera recursos.
     */
    virtual types::EngineError shutdown() = 0;

    /**
     * @brief Retorna o estado atual do lifecycle.
     */
    [[nodiscard]] virtual types::EngineState state() const noexcept = 0;

    /**
     * @brief Retorna a configuração carregada no bootstrap.
     */
    [[nodiscard]] virtual const types::EngineConfig& config() const noexcept = 0;
};

}  // namespace vme::engine::core::contracts
