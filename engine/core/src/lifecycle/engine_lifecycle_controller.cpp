/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

// Implementação padrão da máquina de lifecycle do core.
#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

#include <string>
#include <vector>

#include "engine/core/types/engine_error_codes.hpp"

namespace vme::engine::core::lifecycle {

// Aliases locais para reduzir verbosidade e manter legibilidade no .cpp.
using types::EngineConfig;
using types::EngineError;
using types::EngineErrorSeverity;
using types::EngineState;
using types::FrameContext;

namespace {
constexpr char kLifecycleModule[] = "engine.core.lifecycle";
}  // namespace

EngineLifecycleController::EngineLifecycleController(contracts::IEngineLogger* logger) noexcept
    : logger_ {logger} {}

void EngineLifecycleController::set_logger(contracts::IEngineLogger* logger) noexcept {
    std::scoped_lock lock {mutex_};
    logger_ = logger;
}

// Realiza bootstrap inicial.
EngineError EngineLifecycleController::initialize(const EngineConfig& config) {
    // Lock de escopo para garantir atomicidade das transições.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;

    // Rejeita bootstrap fora do estado inicial para evitar reinit inconsistente.
    if (state_ != EngineState::Uninitialized) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecycleInitializeInvalidState,
            types::lifecycle_error::LifecycleInitializeInvalidStateMessage);
        log_lifecycle_event("initialize", state_before, state_, error);
        return error;
    }

    // Valida invariantes mínimas da configuração antes de aplicá-la.
    if (!config.is_valid()) {
        const auto error = EngineError {
            .code = types::lifecycle_error::LifecycleInvalidConfig,
            .severity = EngineErrorSeverity::Recoverable,
            .message = types::lifecycle_error::LifecycleInvalidConfigMessage,
        };
        log_lifecycle_event("initialize", state_before, state_, error);
        return error;
    }

    // Persiste configuração efetiva para futuras consultas diagnósticas.
    config_ = config;
    // Marca bootstrap concluído.
    state_ = EngineState::Initialized;

    // Retorno de sucesso.
    const auto result = EngineError {};
    log_lifecycle_event("initialize", state_before, state_, result);
    return result;
}

// Processa um frame.
EngineError EngineLifecycleController::tick(const FrameContext& frame_context) {
    // Lock para garantir leitura/escrita consistente de estado e contexto interno.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;

    // Tick é permitido somente após bootstrap e durante execução.
    if (state_ != EngineState::Initialized && state_ != EngineState::Running) {
        const auto error = invalid_transition_error(
            types::lifecycle_error::LifecycleTickInvalidState,
            types::lifecycle_error::LifecycleTickInvalidStateMessage);
        log_lifecycle_event("tick", state_before, state_, error);
        return error;
    }

    // Delta negativo viola invariantes temporais do frame loop.
    if (frame_context.delta_time < FrameContext::Clock::duration::zero()) {
        const auto error = EngineError {
            .code = types::lifecycle_error::LifecycleInvalidFrameContext,
            .severity = EngineErrorSeverity::Recoverable,
            .message = types::lifecycle_error::LifecycleInvalidFrameContextMessage,
        };
        log_lifecycle_event("tick", state_before, state_, error);
        return error;
    }

    // Primeira chamada de tick válida após initialize promove o estado para running.
    if (state_ == EngineState::Initialized) {
        state_ = EngineState::Running;
    }

    // Sem trabalho adicional nesta fase: sucesso no tick.
    const auto result = EngineError {};
    log_lifecycle_event("tick", state_before, state_, result);
    return result;
}

// Pausa execução sem desligar runtime.
EngineError EngineLifecycleController::pause() {
    // Lock protege transição de estado.
    std::scoped_lock lock {mutex_};

    // Pausa só é válida durante execução ativa.
    if (state_ != EngineState::Running) {
        return invalid_transition_error(
            types::lifecycle_error::LifecyclePauseInvalidState,
            types::lifecycle_error::LifecyclePauseInvalidStateMessage);
    }

    // Aplica transição running -> paused.
    state_ = EngineState::Paused;
    return EngineError {};
}

// Retoma execução após pausa.
EngineError EngineLifecycleController::resume() {
    // Lock protege transição de estado.
    std::scoped_lock lock {mutex_};

    // Resume só é válido a partir de paused.
    if (state_ != EngineState::Paused) {
        return invalid_transition_error(
            types::lifecycle_error::LifecycleResumeInvalidState,
            types::lifecycle_error::LifecycleResumeInvalidStateMessage);
    }

    // Aplica transição paused -> running.
    state_ = EngineState::Running;
    return EngineError {};
}

// Encerra runtime de forma ordenada.
EngineError EngineLifecycleController::shutdown() {
    // Lock para serializar mudança de estado durante encerramento.
    std::scoped_lock lock {mutex_};

    const EngineState state_before = state_;

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
            log_lifecycle_event("shutdown", state_before, state_, result);
            return result;
        }
        // Não há shutdown válido sem bootstrap prévio.
        case EngineState::Uninitialized: {
            const auto error = invalid_transition_error(
                types::lifecycle_error::LifecycleShutdownInvalidState,
                types::lifecycle_error::LifecycleShutdownInvalidStateMessage);
            log_lifecycle_event("shutdown", state_before, state_, error);
            return error;
        }
    }

    // Finaliza parada.
    state_ = EngineState::Stopped;
    const auto result = EngineError {};
    log_lifecycle_event("shutdown", state_before, state_, result);
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

void EngineLifecycleController::log_lifecycle_event(
    const char* operation,
    const EngineState state_before,
    const EngineState state_after,
    const EngineError& result) const {
    if (logger_ == nullptr) {
        return;
    }

    auto payload = types::EngineLogPayload {
        .module = kLifecycleModule,
        .severity = result.severity,
        .event = operation,
        .message = result.is_ok() ? "Lifecycle operation completed." : result.message,
        .fields = std::vector<types::EngineLogField> {
            types::EngineLogField {"state_before", std::string {types::to_string(state_before)}},
            types::EngineLogField {"state_after", std::string {types::to_string(state_after)}},
            types::EngineLogField {"result_code", std::to_string(result.code)},
            types::EngineLogField {"severity", std::string {types::to_string(result.severity)}},
        },
    };

    logger_->log(payload);
}

}  // namespace vme::engine::core::lifecycle
