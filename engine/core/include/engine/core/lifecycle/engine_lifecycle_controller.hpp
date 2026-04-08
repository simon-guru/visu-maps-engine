/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// cstdint para códigos de erro padronizados.
#include <cstdint>
// mutex para garantir segurança em acesso concorrente ao estado interno.
#include <mutex>

// Interface pública que será implementada por este controller.
#include "engine/core/contracts/i_engine_lifecycle.hpp"

namespace vme::engine::core::lifecycle {

/**
 * @brief Implementação padrão do lifecycle do runtime.
 *
 * Responsabilidades:
 * - proteger estado interno com sincronização simples;
 * - validar transições para evitar uso incorreto da API;
 * - devolver erros consistentes para integração previsível.
 */
class EngineLifecycleController final : public contracts::IEngineLifecycle {
public:
    // Construtor padrão suficiente: estado inicial é definido por membros.
    EngineLifecycleController() = default;
    // Destrutor virtual herdado da interface; aqui permanece padrão.
    ~EngineLifecycleController() override = default;

    // Bootstrap inicial do runtime.
    types::EngineError initialize(const types::EngineConfig& config) override;
    // Execução de frame.
    types::EngineError tick(const types::FrameContext& frame_context) override;
    // Transição para pausa.
    types::EngineError pause() override;
    // Retomada após pausa.
    types::EngineError resume() override;
    // Encerramento ordenado.
    types::EngineError shutdown() override;

    // Consulta de estado atual.
    [[nodiscard]] types::EngineState state() const noexcept override;
    // Consulta de configuração carregada.
    [[nodiscard]] const types::EngineConfig& config() const noexcept override;

private:
    // Fábrica interna para erros de transição inválida com semântica uniforme.
    [[nodiscard]] types::EngineError invalid_transition_error(
        std::uint32_t code,
        const char* message,
        types::EngineErrorSeverity severity = types::EngineErrorSeverity::Recoverable) const;

    // Mutex que protege leitura/escrita de `state_` e `config_`.
    mutable std::mutex mutex_ {};
    // Configuração efetiva após initialize.
    types::EngineConfig config_ {};
    // Estado da máquina de lifecycle.
    types::EngineState state_ {types::EngineState::Uninitialized};
};

}  // namespace vme::engine::core::lifecycle
