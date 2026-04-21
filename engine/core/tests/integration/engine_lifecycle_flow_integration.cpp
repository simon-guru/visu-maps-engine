#include <cassert>
#include <chrono>

#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

namespace {

using vme::engine::core::lifecycle::EngineLifecycleController;
using vme::engine::core::types::EngineConfig;
using vme::engine::core::types::EngineState;
using vme::engine::core::types::FrameContext;

FrameContext make_frame(const std::uint64_t frame_index) {
    return FrameContext {
        .frame_index = frame_index,
        .delta_time = std::chrono::milliseconds {16},
        .timestamp = FrameContext::Clock::time_point {std::chrono::milliseconds {16 * frame_index}},
    };
}

}  // namespace

int main() {
    EngineLifecycleController controller {};

    const auto config = EngineConfig {
        .app_name = "integration-lifecycle-flow",
        .target_fps = 60,
        .enable_validation = true,
        .enable_frame_trace = false,
    };

    assert(controller.initialize(config).is_ok());
    assert(controller.state() == EngineState::Initialized);

    assert(controller.tick(make_frame(1)).is_ok());
    assert(controller.state() == EngineState::Running);

    assert(controller.pause().is_ok());
    assert(controller.state() == EngineState::Paused);

    const auto tick_while_paused = controller.tick(make_frame(2));
    assert(!tick_while_paused.is_ok());
    assert(tick_while_paused.code == 1003U);
    assert(controller.state() == EngineState::Paused);

    assert(controller.resume().is_ok());
    assert(controller.state() == EngineState::Running);

    assert(controller.tick(make_frame(3)).is_ok());
    assert(controller.state() == EngineState::Running);

    assert(controller.shutdown().is_ok());
    assert(controller.state() == EngineState::Stopped);

    const auto repeated_shutdown = controller.shutdown();
    assert(repeated_shutdown.is_ok());
    assert(repeated_shutdown.code == 0U);
    assert(controller.state() == EngineState::Stopped);

    return 0;
}
