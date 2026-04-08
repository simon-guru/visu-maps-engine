/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::types {

/**
 * @brief Operações suportadas pela máquina de lifecycle.
 */
enum class EngineLifecycleOperation : std::uint8_t {
    Initialize = 0,
    Tick,
    Pause,
    Resume,
    Shutdown,
};

/**
 * @brief Fase do evento emitido para cada operação de lifecycle.
 */
enum class EngineLifecycleEventPhase : std::uint8_t {
    Attempt = 0,
    Success,
    Failure,
};

/**
 * @brief Evento canônico de observabilidade para operações de lifecycle.
 */
struct EngineLifecycleEvent final {
    EngineLifecycleOperation operation {EngineLifecycleOperation::Initialize};
    EngineLifecycleEventPhase phase {EngineLifecycleEventPhase::Attempt};
    EngineState state_before {EngineState::Uninitialized};
    EngineState state_after {EngineState::Uninitialized};
    std::uint32_t error_code {0U};
    std::string error_message {};
};

/**
 * @brief Converte operação de lifecycle para string estável de telemetria.
 */
[[nodiscard]] constexpr std::string_view to_string(
    const EngineLifecycleOperation operation) noexcept {
    switch (operation) {
        case EngineLifecycleOperation::Initialize:
            return "initialize";
        case EngineLifecycleOperation::Tick:
            return "tick";
        case EngineLifecycleOperation::Pause:
            return "pause";
        case EngineLifecycleOperation::Resume:
            return "resume";
        case EngineLifecycleOperation::Shutdown:
            return "shutdown";
    }

    return "unknown";
}

/**
 * @brief Converte fase do evento para string estável de telemetria.
 */
[[nodiscard]] constexpr std::string_view to_string(
    const EngineLifecycleEventPhase phase) noexcept {
    switch (phase) {
        case EngineLifecycleEventPhase::Attempt:
            return "attempt";
        case EngineLifecycleEventPhase::Success:
            return "success";
        case EngineLifecycleEventPhase::Failure:
            return "failure";
    }

    return "unknown";
}

}  // namespace vme::engine::core::types
