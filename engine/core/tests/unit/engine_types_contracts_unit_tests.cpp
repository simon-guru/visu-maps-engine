#include <cassert>
#include <chrono>
#include <cstdint>
#include <string_view>

#include "engine/core/types/engine_models.hpp"

namespace {

using vme::engine::core::types::EngineConfig;
using vme::engine::core::types::EngineError;
using vme::engine::core::types::EngineErrorSeverity;
using vme::engine::core::types::EngineState;
using vme::engine::core::types::FrameContext;
using vme::engine::core::types::to_string;

void test_engine_config_validation_contract() {
    const auto valid_config = EngineConfig {
        .app_name = "core-unit-tests",
        .target_fps = 60,
        .enable_validation = true,
        .enable_frame_trace = false,
    };
    assert(valid_config.is_valid());

    const auto empty_name = EngineConfig {
        .app_name = "",
        .target_fps = 60,
        .enable_validation = false,
        .enable_frame_trace = false,
    };
    assert(!empty_name.is_valid());

    const auto invalid_fps = EngineConfig {
        .app_name = "core-unit-tests",
        .target_fps = 0,
        .enable_validation = false,
        .enable_frame_trace = false,
    };
    assert(!invalid_fps.is_valid());
}

void test_engine_error_success_contract() {
    const auto success = EngineError {
        .code = 0,
        .severity = EngineErrorSeverity::Info,
        .message = "",
    };
    assert(success.is_ok());

    const auto failure = EngineError {
        .code = 42,
        .severity = EngineErrorSeverity::Recoverable,
        .message = "contract failure",
    };
    assert(!failure.is_ok());
}

void test_state_and_severity_string_serialization() {
    assert(to_string(EngineState::Uninitialized) == std::string_view {"uninitialized"});
    assert(to_string(EngineState::Initialized) == std::string_view {"initialized"});
    assert(to_string(EngineState::Running) == std::string_view {"running"});
    assert(to_string(EngineState::Paused) == std::string_view {"paused"});
    assert(to_string(EngineState::Stopping) == std::string_view {"stopping"});
    assert(to_string(EngineState::Stopped) == std::string_view {"stopped"});

    const auto invalid_state = static_cast<EngineState>(255);
    assert(to_string(invalid_state) == std::string_view {"unknown"});

    assert(to_string(EngineErrorSeverity::Info) == std::string_view {"info"});
    assert(to_string(EngineErrorSeverity::Warning) == std::string_view {"warning"});
    assert(to_string(EngineErrorSeverity::Recoverable) == std::string_view {"recoverable"});
    assert(to_string(EngineErrorSeverity::Fatal) == std::string_view {"fatal"});

    const auto invalid_severity = static_cast<EngineErrorSeverity>(255);
    assert(to_string(invalid_severity) == std::string_view {"unknown"});
}

void test_frame_context_default_contract() {
    const FrameContext frame {};
    assert(frame.frame_index == 0U);
    assert(frame.delta_time == FrameContext::Clock::duration::zero());
}

}  // namespace

int main() {
    test_engine_config_validation_contract();
    test_engine_error_success_contract();
    test_state_and_severity_string_serialization();
    test_frame_context_default_contract();
    return 0;
}
