/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

namespace vme::engine::core::lifecycle {

using types::EngineConfig;
using types::EngineError;
using types::EngineErrorSeverity;
using types::EngineState;
using types::FrameContext;

EngineError EngineLifecycleController::initialize(const EngineConfig& config) {
    std::scoped_lock lock {mutex_};

    if (state_ != EngineState::Uninitialized) {
        return invalid_transition_error(1001U, "initialize requer estado Uninitialized.");
    }

    if (!config.is_valid()) {
        return EngineError {
            .code = 1002U,
            .severity = EngineErrorSeverity::Recoverable,
            .message = "EngineConfig inválido: app_name vazio ou target_fps <= 0.",
        };
    }

    config_ = config;
    state_ = EngineState::Initialized;
    return EngineError {};
}

EngineError EngineLifecycleController::tick(const FrameContext& frame_context) {
    std::scoped_lock lock {mutex_};

    if (state_ == EngineState::Initialized) {
        state_ = EngineState::Running;
    }

    if (state_ != EngineState::Running) {
        return invalid_transition_error(1003U, "tick requer estado Running ou Initialized.");
    }

    if (frame_context.delta_time < FrameContext::Clock::duration::zero()) {
        return EngineError {
            .code = 1004U,
            .severity = EngineErrorSeverity::Recoverable,
            .message = "FrameContext inválido: delta_time negativo.",
        };
    }

    return EngineError {};
}

EngineError EngineLifecycleController::pause() {
    std::scoped_lock lock {mutex_};

    if (state_ != EngineState::Running) {
        return invalid_transition_error(1006U, "pause requer estado Running.");
    }

    state_ = EngineState::Paused;
    return EngineError {};
}

EngineError EngineLifecycleController::resume() {
    std::scoped_lock lock {mutex_};

    if (state_ != EngineState::Paused) {
        return invalid_transition_error(1007U, "resume requer estado Paused.");
    }

    state_ = EngineState::Running;
    return EngineError {};
}

EngineError EngineLifecycleController::shutdown() {
    std::scoped_lock lock {mutex_};

    switch (state_) {
        case EngineState::Initialized:
        case EngineState::Running:
        case EngineState::Paused:
            state_ = EngineState::Stopping;
            break;
        case EngineState::Stopping:
        case EngineState::Stopped:
            return EngineError {};
        case EngineState::Uninitialized:
            return invalid_transition_error(1005U, "shutdown requer engine inicializada.");
    }

    state_ = EngineState::Stopped;
    return EngineError {};
}

EngineState EngineLifecycleController::state() const noexcept {
    std::scoped_lock lock {mutex_};
    return state_;
}

const EngineConfig& EngineLifecycleController::config() const noexcept {
    std::scoped_lock lock {mutex_};
    return config_;
}

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

}  // namespace vme::engine::core::lifecycle
