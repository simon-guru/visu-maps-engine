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

// Interfaces públicas consumidas pelo controller.
#include "engine/core/contracts/i_engine_lifecycle.hpp"
#include "engine/core/contracts/i_engine_logger.hpp"
#include "engine/core/contracts/i_engine_tracer.hpp"
#include "engine/core/types/engine_lifecycle_events.hpp"

namespace vme::engine::core::lifecycle {

/**
 * @brief Implementação padrão do lifecycle do runtime.
 *
 * Responsabilidades:
 * - proteger estado interno com sincronização simples;
 * - validar transições para evitar uso incorreto da API;
 * - devolver erros consistentes para integração previsível;
 * - emitir logs estruturados quando um logger opcional estiver configurado.
 */
class EngineLifecycleController final : public contracts::IEngineLifecycle {
public:
    // Construtor opcionalmente recebe logger não-proprietário.
    explicit EngineLifecycleController(
        contracts::IEngineLogger* logger = nullptr,
        contracts::IEngineTracer* tracer = nullptr) noexcept;
    // Destrutor virtual herdado da interface; aqui permanece padrão.
    ~EngineLifecycleController() override = default;

    // Atualiza logger opcional em runtime. Ponteiro nulo desabilita emissão.
    void set_logger(contracts::IEngineLogger* logger) noexcept;

    // Atualiza tracer opcional em runtime. Ponteiro nulo desabilita emissão.
    void set_tracer(contracts::IEngineTracer* tracer) noexcept;

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

    // Emite evento estruturado de lifecycle no pipeline de observabilidade.
    // No-op quando logger_ == nullptr.
    void emit_lifecycle_event(const types::EngineLifecycleEvent& event) const;


    // Emite trace de frame quando habilitado por configuração e tracer disponível.
    void trace_frame_event(const types::FrameContext& frame_context) const;

    // Mutex que protege leitura/escrita de `state_`, `config_`, `logger_` e `tracer_`.
    mutable std::mutex mutex_ {};
    // Dependência opcional de logging (não proprietária).
    contracts::IEngineLogger* logger_ {nullptr};
    // Dependência opcional de tracing de frame (não proprietária).
    contracts::IEngineTracer* tracer_ {nullptr};
    // Configuração efetiva após initialize.
    types::EngineConfig config_ {};
    // Estado da máquina de lifecycle.
    types::EngineState state_ {types::EngineState::Uninitialized};
};

}  // namespace vme::engine::core::lifecycle
