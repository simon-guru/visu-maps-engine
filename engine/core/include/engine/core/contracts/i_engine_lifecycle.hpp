/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// Dependência dos modelos base de configuração, estado e erro.
// Motivo: manter o contrato de lifecycle autocontido e semanticamente explícito.
#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato base para governar o ciclo de vida do runtime.
 *
 * Responsabilidade:
 * - padronizar a sequência operacional comum para qualquer host da engine;
 * - reduzir acoplamento entre módulos por meio de uma capacidade mínima e explícita;
 * - permitir múltiplas implementações concretas mantendo API estável.
 */
class IEngineLifecycle {
public:
    // Destrutor virtual para garantir destruição correta via ponteiro para interface.
    virtual ~IEngineLifecycle() = default;

    /**
     * @brief Inicializa runtime com configuração validável.
     * @param config Configuração de bootstrap.
     * @return EngineError com `code == 0` em caso de sucesso.
     */
    virtual types::EngineError initialize(const types::EngineConfig& config) = 0;

    /**
     * @brief Processa um frame/tick da engine.
     * @param frame_context Metadados temporais do frame atual.
     * @return EngineError com resultado da operação.
     */
    virtual types::EngineError tick(const types::FrameContext& frame_context) = 0;

    /**
     * @brief Move runtime para estado de pausa sem desalocar recursos críticos.
     */
    virtual types::EngineError pause() = 0;

    /**
     * @brief Retoma execução normal a partir de estado pausado.
     */
    virtual types::EngineError resume() = 0;

    /**
     * @brief Solicita encerramento ordenado do runtime.
     */
    virtual types::EngineError shutdown() = 0;

    /**
     * @brief Fornece o estado atual da máquina de lifecycle.
     */
    [[nodiscard]] virtual types::EngineState state() const noexcept = 0;

    /**
     * @brief Exibe a configuração efetiva carregada no bootstrap.
     *
     * Motivo: permitir inspeção operacional e diagnóstica sem duplicar estado
     * fora do controller.
     */
    [[nodiscard]] virtual const types::EngineConfig& config() const noexcept = 0;
};

}  // namespace vme::engine::core::contracts
