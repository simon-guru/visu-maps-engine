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
#include <vector>

// Interfaces públicas consumidas pelo controller.
#include "engine/core/contracts/i_engine_lifecycle.hpp"
#include "engine/core/contracts/i_engine_logger.hpp"
#include "engine/core/contracts/i_engine_tracer.hpp"
#include "engine/core/contracts/i_observability_sink.hpp"
#include "engine/core/contracts/noop_observability_sink.hpp"
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

    // Registra sink de observabilidade não-proprietário.
    void add_observability_sink(contracts::IObservabilitySink* sink) noexcept;

    // Remove sink de observabilidade previamente registrado.
    void remove_observability_sink(contracts::IObservabilitySink* sink) noexcept;

    // Remove todos os sinks customizados e mantém fallback no-op.
    void clear_observability_sinks() noexcept;

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

    // Emite envelope canônico para todos os sinks registrados.
    void export_observability_signal(types::EngineObservabilitySignal signal) const;

    // Emite evento estruturado de lifecycle no pipeline de observabilidade.
    // No-op para logging quando logger_ == nullptr e para export quando não houver sink customizado.
    void emit_lifecycle_event(const types::EngineLifecycleEvent& event) const;

    // Emite trace de frame quando habilitado por configuração e tracer disponível.
    void trace_frame_event(const types::FrameContext& frame_context) const;

    // Mutex que protege leitura/escrita de estado interno e registro de observabilidade.
    mutable std::mutex state_mutex_ {}, sinks_mutex_ {};
    // Dependência opcional de logging (não proprietária).
    contracts::IEngineLogger* logger_ {nullptr};
    // Dependência opcional de tracing de frame (não proprietária).
    contracts::IEngineTracer* tracer_ {nullptr};
    // Configuração efetiva após initialize.
    types::EngineConfig config_ {};
    // Fallback no-op para ambiente sem observabilidade.
    // O uso de um sink no-op garante que o controller possa emitir sinais de observabilidade sem a necessidade de verificar a presença de sinks registrados, simplificando a lógica de emissão e garantindo que os sinais sejam descartados silenciosamente quando não houver sinks personalizados, evitando a necessidade de verificações adicionais ou tratamento especial para o caso sem sinks.
    contracts::NoopObservabilitySink noop_sink_ {};
    // Coleção de sinks de exportação (não-proprietária).
    // O uso de ponteiros crus é intencional para evitar acoplamento de propriedade e facilitar integração com sinks gerenciados externamente. O controller não é responsável por gerenciar a memória dos sinks; o caller deve garantir que os sinks sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados. O método add_observability_sink deve garantir que os sinks sejam registrados corretamente, permitindo múltiplos sinks simultâneos, e que a lista de sinks seja protegida contra condições de corrida usando mutexes para garantir a thread-safety.
    // O método remove_observability_sink deve garantir que os sinks sejam removidos corretamente, garantindo que eles não recebam mais sinais futuros, e que a lista de sinks seja protegida contra condições de corrida usando mutexes para garantir a thread-safety. O método clear_observability_sinks deve garantir que todos os sinks sejam removidos corretamente, garantindo que eles não recebam mais sinais futuros, e que a lista de sinks seja protegida contra condições de corrida usando mutexes para garantir a thread-safety.
    std::vector<contracts::IObservabilitySink*> observability_sinks_ {&noop_sink_};
    // Estado da máquina de lifecycle.
    types::EngineState state_ {types::EngineState::Uninitialized};
};

}  // namespace vme::engine::core::lifecycle
