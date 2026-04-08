/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "engine/core/types/engine_lifecycle_events.hpp"
#include "engine/core/types/engine_logging.hpp"
#include "engine/core/types/engine_trace_events.hpp"

namespace vme::engine::core::types {

/**
 * @brief Tipo discriminado canônico para exportação de sinais de observabilidade.
 */
using EngineObservabilitySignal =
    std::variant<EngineLogPayload, FrameTraceEvent, EngineLifecycleEvent>;

/**
 * @brief Envelope canônico para exportação de observabilidade do core.
 */
struct EngineObservabilityEnvelope final {
    // Momento de emissão em nanossegundos (clock monotônico da engine).
    std::int64_t emitted_at_ns {0};
    // Módulo de origem do evento para roteamento em sinks externos.
    std::string source {"engine.core"};
    // Sinal efetivo (log, trace ou lifecycle).
    EngineObservabilitySignal signal {};
};

}  // namespace vme::engine::core::types
