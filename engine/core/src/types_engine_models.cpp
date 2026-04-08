/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

// Implementações concretas dos helpers de serialização textual dos tipos base.
#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::types {

// Serializa `EngineState` para texto estável em logs, métricas e debug tools.
std::string_view to_string(const EngineState state) noexcept {
    switch (state) {
        // Estado antes do bootstrap.
        case EngineState::Uninitialized:
            return "uninitialized";
        // Estado pronto para iniciar execução de frames.
        case EngineState::Initialized:
            return "initialized";
        // Estado de execução normal.
        case EngineState::Running:
            return "running";
        // Estado de pausa operacional.
        case EngineState::Paused:
            return "paused";
        // Estado de encerramento em curso.
        case EngineState::Stopping:
            return "stopping";
        // Estado finalizado.
        case EngineState::Stopped:
            return "stopped";
    }

    // Fallback defensivo para valores fora da enumeração esperada.
    return "unknown";
}

// Serializa `EngineErrorSeverity` para texto estável de observabilidade.
std::string_view to_string(const EngineErrorSeverity severity) noexcept {
    switch (severity) {
        // Informação não crítica.
        case EngineErrorSeverity::Info:
            return "info";
        // Aviso operacional.
        case EngineErrorSeverity::Warning:
            return "warning";
        // Erro recuperável.
        case EngineErrorSeverity::Recoverable:
            return "recoverable";
        // Erro fatal.
        case EngineErrorSeverity::Fatal:
            return "fatal";
    }

    // Fallback defensivo para valores fora da enumeração esperada.
    return "unknown";
}

}  // namespace vme::engine::core::types
