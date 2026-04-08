#include <cassert>
#include <chrono>
#include <variant>
#include <vector>

#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

namespace {
using vme::engine::core::contracts::IEngineLogger;
using vme::engine::core::contracts::IEngineTracer;
using vme::engine::core::contracts::IObservabilitySink;
using vme::engine::core::lifecycle::EngineLifecycleController;
using vme::engine::core::types::EngineConfig;
using vme::engine::core::types::EngineLifecycleEvent;
using vme::engine::core::types::EngineObservabilityEnvelope;
using vme::engine::core::types::EngineState;
using vme::engine::core::types::FrameContext;
using vme::engine::core::types::FrameTraceEvent;

class BufferingLogger final : public IEngineLogger {
public:
    void log(const vme::engine::core::types::EngineLogPayload& payload) override {
        payloads.push_back(payload);
    }

    std::vector<vme::engine::core::types::EngineLogPayload> payloads {};
};

class BufferingTracer final : public IEngineTracer {
public:
    void trace_frame(const FrameTraceEvent& event) override { events.push_back(event); }
    std::vector<FrameTraceEvent> events {};
};

class BufferingSink final : public IObservabilitySink {
public:
    void export_signal(const EngineObservabilityEnvelope& envelope) override {
        envelopes.push_back(envelope);
    }

    std::vector<EngineObservabilityEnvelope> envelopes {};
};

}  // namespace

int main() {
    BufferingLogger logger {};
    BufferingTracer tracer {};
    BufferingSink sink {};

    EngineLifecycleController controller {&logger, &tracer};
    controller.add_observability_sink(&sink);

    const auto config = EngineConfig {
        .app_name = "integration-observability-enabled",
        .target_fps = 120,
        .enable_validation = true,
        .enable_frame_trace = true,
    };

    assert(controller.initialize(config).is_ok());

    for (std::uint64_t i = 0; i < 3; ++i) {
        const auto frame = FrameContext {
            .frame_index = i,
            .delta_time = std::chrono::milliseconds {16},
            .timestamp = FrameContext::Clock::time_point {std::chrono::milliseconds {16 * i}},
        };
        assert(controller.tick(frame).is_ok());
    }

    assert(controller.shutdown().is_ok());
    assert(controller.state() == EngineState::Stopped);

    std::size_t lifecycle_count = 0;
    std::size_t trace_count = 0;
    for (const auto& envelope : sink.envelopes) {
        lifecycle_count += std::holds_alternative<EngineLifecycleEvent>(envelope.signal) ? 1U : 0U;
        trace_count += std::holds_alternative<FrameTraceEvent>(envelope.signal) ? 1U : 0U;
    }

    assert(lifecycle_count == 10);
    assert(trace_count == 3);
    assert(tracer.events.size() == 3);
    assert(logger.payloads.size() == 10);

    return 0;
}
