/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

// Implementação padrão da máquina de lifecycle do core.
#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

#include <algorithm>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include "engine/core/types/engine_error_codes.hpp"

namespace vme::engine::core::lifecycle {

// Aliases locais para reduzir verbosidade e manter legibilidade no .cpp.
using types::EngineConfig;
using types::EngineError;
using types::EngineErrorSeverity;
using types::EngineLifecycleEvent;
using types::EngineLifecycleEventPhase;
using types::EngineLifecycleOperation;
using types::EngineState;
using types::FrameContext;

namespace {
constexpr char kLifecycleModule[] = "engine.core.lifecycle";
}  // namespace

EngineLifecycleController::EngineLifecycleController(
    contracts::IEngineLogger* logger, contracts::IEngineTracer* tracer) noexcept
    : logger_ {logger}, tracer_ {tracer} {}

void EngineLifecycleController::set_logger(contracts::IEngineLogger* logger) noexcept {
    std::scoped_lock lock {mutex_};
    logger_ = logger;
}

void EngineLifecycleController::set_tracer(contracts::IEngineTracer* tracer) noexcept {
    std::scoped_lock lock {mutex_};
    tracer_ = tracer;
}


void EngineLifecycleController::add_observability_sink(contracts::IObservabilitySink* sink) noexcept {
    if (sink == nullptr || sink == &noop_sink_) {
        return;
    }

    std::scoped_lock lock {mutex_};
    if (std::find(observability_sinks_.begin(), observability_sinks_.end(), sink)
        != observability_sinks_.end()) {
        return;
    }

    if (observability_sinks_.size() == 1U && observability_sinks_.front() == &noop_sink_) {
        observability_sinks_.clear();
    }

    observability_sinks_.push_back(sink);
}

void EngineLifecycleController::remove_observability_sink(contracts::IObservabilitySink* sink) noexcept {
    std::scoped_lock lock {mutex_};
    const auto new_end = std::remove(observability_sinks_.begin(), observability_sinks_.end(), sink);
    observability_sinks_.erase(new_end, observability_sinks_.end());

    if (observability_sinks_.empty()) {
        observability_sinks_.push_back(&noop_sink_);
    }
}

void EngineLifecycleController::clear_observability_sinks() noexcept {
    std::scoped_lock lock {mutex_};
    observability_sinks_.clear();
    observability_sinks_.push_back(&noop_sink_);
}

// Realiza bootstrap inicial.
EngineError EngineLifecycleController::initialize(const EngineConfig& config) {
    // Lock de escopo para garantir atomicidade das transições.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Initialize,
        .phase = EngineLifecycleEventPhase::Attempt,
        .state_before = state_before,
        .state_after = state_before,
    });

    // Rejeita bootstrap fora do estado inicial para evitar reinit inconsistente.
    if (state_ != EngineState::Uninitialized) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecycleInitializeInvalidState,
            types::lifecycle_error::LifecycleInitializeInvalidStateMessage);
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Initialize,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Valida invariantes mínimas da configuração antes de aplicá-la.
    if (!config.is_valid()) {
        const auto error = EngineError {
            .code = types::lifecycle_error::LifecycleInvalidConfig,
            .severity = EngineErrorSeverity::Recoverable,
            .message = types::lifecycle_error::LifecycleInvalidConfigMessage,
        };
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Initialize,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Persiste configuração efetiva para futuras consultas diagnósticas.
    config_ = config;
    // Marca bootstrap concluído.
    state_ = EngineState::Initialized;

    // Retorno de sucesso.
    const auto result = EngineError {};
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Initialize,
        .phase = EngineLifecycleEventPhase::Success,
        .state_before = state_before,
        .state_after = state_,
    });
    return result;
}

// Processa um frame.
EngineError EngineLifecycleController::tick(const FrameContext& frame_context) {
    // Lock para garantir leitura/escrita consistente de estado e contexto interno.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Tick,
        .phase = EngineLifecycleEventPhase::Attempt,
        .state_before = state_before,
        .state_after = state_before,
    });

    // Tick é permitido somente após bootstrap e durante execução.
    if (state_ != EngineState::Initialized && state_ != EngineState::Running) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecycleTickInvalidState,
            types::lifecycle_error::LifecycleTickInvalidStateMessage);
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Tick,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Delta negativo viola invariantes temporais do frame loop.
    if (frame_context.delta_time < FrameContext::Clock::duration::zero()) {
        const auto error = EngineError {
            .code = types::lifecycle_error::LifecycleInvalidFrameContext,
            .severity = EngineErrorSeverity::Recoverable,
            .message = types::lifecycle_error::LifecycleInvalidFrameContextMessage,
        };
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Tick,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Primeira chamada de tick válida após initialize promove o estado para running.
    if (state_ == EngineState::Initialized) {
        state_ = EngineState::Running;
    }

    // Tracing de frame é emitido apenas quando explicitamente habilitado.
    trace_frame_event(frame_context);

    // Sem trabalho adicional nesta fase: sucesso no tick.
    const auto result = EngineError {};
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Tick,
        .phase = EngineLifecycleEventPhase::Success,
        .state_before = state_before,
        .state_after = state_,
    });
    return result;
}

// Pausa execução sem desligar runtime.
EngineError EngineLifecycleController::pause() {
    // Lock protege transição de estado.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Pause,
        .phase = EngineLifecycleEventPhase::Attempt,
        .state_before = state_before,
        .state_after = state_before,
    });

    // Pausa só é válida durante execução ativa.
    if (state_ != EngineState::Running) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecyclePauseInvalidState,
            types::lifecycle_error::LifecyclePauseInvalidStateMessage);
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Pause,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Aplica transição running -> paused.
    state_ = EngineState::Paused;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Pause,
        .phase = EngineLifecycleEventPhase::Success,
        .state_before = state_before,
        .state_after = state_,
    });
    return EngineError {};
}

// Retoma execução após pausa.
EngineError EngineLifecycleController::resume() {
    // Lock protege transição de estado.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Resume,
        .phase = EngineLifecycleEventPhase::Attempt,
        .state_before = state_before,
        .state_after = state_before,
    });

    // Resume só é válido a partir de paused.
    if (state_ != EngineState::Paused) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecycleResumeInvalidState,
            types::lifecycle_error::LifecycleResumeInvalidStateMessage);
        emit_lifecycle_event(EngineLifecycleEvent {
            .operation = EngineLifecycleOperation::Resume,
            .phase = EngineLifecycleEventPhase::Failure,
            .state_before = state_before,
            .state_after = state_,
            .error_code = error.code,
            .error_message = error.message,
        });
        return error;
    }

    // Aplica transição paused -> running.
    state_ = EngineState::Running;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Resume,
        .phase = EngineLifecycleEventPhase::Success,
        .state_before = state_before,
        .state_after = state_,
    });
    return EngineError {};
}

// Encerra runtime de forma ordenada.
EngineError EngineLifecycleController::shutdown() {
    // Lock para serializar mudança de estado durante encerramento.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Shutdown,
        .phase = EngineLifecycleEventPhase::Attempt,
        .state_before = state_before,
        .state_after = state_before,
    });

    // Define rotas válidas de encerramento.
    switch (state_) {
        // Estados ativos podem iniciar parada.
        case EngineState::Initialized:
        case EngineState::Running:
        case EngineState::Paused:
            state_ = EngineState::Stopping;
            break;
        // Se já está parando/parado, shutdown é tratado como idempotente.
        case EngineState::Stopping:
        case EngineState::Stopped: {
            const auto result = EngineError {};
            emit_lifecycle_event(EngineLifecycleEvent {
                .operation = EngineLifecycleOperation::Shutdown,
                .phase = EngineLifecycleEventPhase::Success,
                .state_before = state_before,
                .state_after = state_,
            });
            return result;
        }
        // Não há shutdown válido sem bootstrap prévio.
        case EngineState::Uninitialized: {
            const auto error = invalid_transition_error(
                types::lifecycle_error::LifecycleShutdownInvalidState,
                types::lifecycle_error::LifecycleShutdownInvalidStateMessage);
            emit_lifecycle_event(EngineLifecycleEvent {
                .operation = EngineLifecycleOperation::Shutdown,
                .phase = EngineLifecycleEventPhase::Failure,
                .state_before = state_before,
                .state_after = state_,
                .error_code = error.code,
                .error_message = error.message,
            });
            return error;
        }
    }

    // Finaliza parada.
    state_ = EngineState::Stopped;
    const auto result = EngineError {};
    emit_lifecycle_event(EngineLifecycleEvent {
        .operation = EngineLifecycleOperation::Shutdown,
        .phase = EngineLifecycleEventPhase::Success,
        .state_before = state_before,
        .state_after = state_,
    });
    return result;
}

// Retorna estado atual de forma thread-safe.
EngineState EngineLifecycleController::state() const noexcept {
    std::scoped_lock lock {mutex_};
    return state_;
}

// Retorna configuração efetiva carregada no bootstrap.
const EngineConfig& EngineLifecycleController::config() const noexcept {
    std::scoped_lock lock {mutex_};
    return config_;
}

// Centraliza construção de erros de transição inválida.
EngineError EngineLifecycleController::invalid_transition_error(
    const std::uint32_t code,
    const char* message,
    const EngineErrorSeverity severity) const {
    return EngineError {
        .code = code,
        .severity = severity,
        .message = message,
    };
}

void EngineLifecycleController::export_observability_signal(
    types::EngineObservabilitySignal signal) const {
    const auto emitted_at_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        FrameContext::Clock::now().time_since_epoch());

    const auto envelope = types::EngineObservabilityEnvelope {
        .emitted_at_ns = emitted_at_ns.count(),
        .source = kLifecycleModule,
        .signal = std::move(signal),
    };

    for (auto* sink : observability_sinks_) {
        if (sink == nullptr) {
            continue;
        }

        sink->export_signal(envelope);
    }
}

void EngineLifecycleController::emit_lifecycle_event(const EngineLifecycleEvent& event) const {
    export_observability_signal(event);

    if (logger_ == nullptr) {
        return;
    }

    const auto event_operation = std::string {types::to_string(event.operation)};
    const auto event_phase = std::string {types::to_string(event.phase)};
    const auto event_name = event_operation + "." + event_phase;
    const auto message = event.phase == EngineLifecycleEventPhase::Failure
        ? event.error_message
        : "Lifecycle operation event emitted.";

    auto payload = types::EngineLogPayload {
        .module = kLifecycleModule,
        .severity = event.phase == EngineLifecycleEventPhase::Failure
            ? EngineErrorSeverity::Recoverable
            : EngineErrorSeverity::Info,
        .event = event_name,
        .message = message,
        .fields = std::vector<types::EngineLogField> {
            types::EngineLogField {"operation", event_operation},
            types::EngineLogField {"phase", event_phase},
            types::EngineLogField {
                "state_before",
                std::string {types::to_string(event.state_before)},
            },
            types::EngineLogField {
                "state_after",
                std::string {types::to_string(event.state_after)},
            },
            types::EngineLogField {"error_code", std::to_string(event.error_code)},
            types::EngineLogField {"error_message", event.error_message},
        },
    };

    logger_->log(payload);
}


void EngineLifecycleController::trace_frame_event(const FrameContext& frame_context) const {
    // Caminho desabilitado: retorna antes de qualquer custo de normalização/alocação.
    if (!config_.enable_frame_trace) {
        return;
    }

    // Formato estável para exportação: nanossegundos inteiros assinados.
    const auto delta_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        frame_context.delta_time);
    const auto timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        frame_context.timestamp.time_since_epoch());

    const auto event = types::FrameTraceEvent {
        .frame_index = frame_context.frame_index,
        .delta_time = delta_time_ns.count(),
        .timestamp = timestamp_ns.count(),
    };

    export_observability_signal(event);

    if (tracer_ != nullptr) {
        tracer_->trace_frame(event);
    }
}

}  // namespace vme::engine::core::lifecycle
