#include "engine/core/types/engine_models.hpp"

namespace visu::engine::core::types {

std::string_view to_string(const EngineState state) noexcept {
    switch (state) {
        case EngineState::Uninitialized:
            return "uninitialized";
        case EngineState::Initialized:
            return "initialized";
        case EngineState::Running:
            return "running";
        case EngineState::Paused:
            return "paused";
        case EngineState::Stopping:
            return "stopping";
        case EngineState::Stopped:
            return "stopped";
    }

    return "unknown";
}

std::string_view to_string(const EngineErrorSeverity severity) noexcept {
    switch (severity) {
        case EngineErrorSeverity::Info:
            return "info";
        case EngineErrorSeverity::Warning:
            return "warning";
        case EngineErrorSeverity::Recoverable:
            return "recoverable";
        case EngineErrorSeverity::Fatal:
            return "fatal";
    }

    return "unknown";
}

}  // namespace visu::engine::core::types
