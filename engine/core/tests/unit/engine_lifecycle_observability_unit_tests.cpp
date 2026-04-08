#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
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
using vme::engine::core::types::EngineLifecycleEventPhase;
using vme::engine::core::types::EngineLifecycleOperation;
using vme::engine::core::types::EngineLogPayload;
using vme::engine::core::types::EngineObservabilityEnvelope;
using vme::engine::core::types::EngineState;
using vme::engine::core::types::FrameContext;
using vme::engine::core::types::FrameTraceEvent;

class SpyLogger final : public IEngineLogger {
public:
    void log(const EngineLogPayload& payload) override { payloads.push_back(payload); }

    std::vector<EngineLogPayload> payloads {};
};

class SpyTracer final : public IEngineTracer {
public:
    void trace_frame(const FrameTraceEvent& event) override { events.push_back(event); }

    std::vector<FrameTraceEvent> events {};
};

class SpySink final : public IObservabilitySink {
public:
    void export_signal(const EngineObservabilityEnvelope& envelope) override {
        envelopes.push_back(envelope);
    }

    std::vector<EngineObservabilityEnvelope> envelopes {};
};

const EngineLogPayload& find_log_by_event(
    const std::vector<EngineLogPayload>& logs,
    const std::string& event_name) {
    for (const auto& log : logs) {
        if (log.event == event_name) {
            return log;
        }
    }

    assert(false && "expected log event was not emitted");
    return logs.front();
}

std::string find_field(const EngineLogPayload& payload, const std::string& key) {
    for (const auto& field : payload.fields) {
        if (field.key == key) {
            return field.value;
        }
    }

    return {};
}

std::vector<EngineLifecycleEvent> collect_lifecycle_events(const SpySink& sink) {
    std::vector<EngineLifecycleEvent> events {};
    events.reserve(sink.envelopes.size());

    for (const auto& envelope : sink.envelopes) {
        if (std::holds_alternative<EngineLifecycleEvent>(envelope.signal)) {
            events.push_back(std::get<EngineLifecycleEvent>(envelope.signal));
        }
    }

    return events;
}

std::vector<FrameTraceEvent> collect_trace_events(const SpySink& sink) {
    std::vector<FrameTraceEvent> events {};
    events.reserve(sink.envelopes.size());

    for (const auto& envelope : sink.envelopes) {
        if (std::holds_alternative<FrameTraceEvent>(envelope.signal)) {
            events.push_back(std::get<FrameTraceEvent>(envelope.signal));
        }
    }

    return events;
}

void test_emits_consistent_observability_for_initialize_tick_shutdown() {
    SpyLogger logger {};
    SpyTracer tracer {};
    SpySink sink {};

    EngineLifecycleController controller {&logger, &tracer};
    controller.add_observability_sink(&sink);

    const auto config = EngineConfig {
        .app_name = "unit-tests",
        .target_fps = 60,
        .enable_validation = true,
        .enable_frame_trace = true,
    };

    const auto initialize_error = controller.initialize(config);
    assert(initialize_error.is_ok());

    const auto frame_context = FrameContext {
        .frame_index = 7,
        .delta_time = std::chrono::milliseconds {16},
        .timestamp = FrameContext::Clock::time_point {std::chrono::seconds {42}},
    };

    const auto tick_error = controller.tick(frame_context);
    assert(tick_error.is_ok());

    const auto shutdown_error = controller.shutdown();
    assert(shutdown_error.is_ok());
    assert(controller.state() == EngineState::Stopped);

    const auto lifecycle_events = collect_lifecycle_events(sink);
    assert(lifecycle_events.size() == 6);

    const auto& initialize_attempt = lifecycle_events[0];
    assert(initialize_attempt.operation == EngineLifecycleOperation::Initialize);
    assert(initialize_attempt.phase == EngineLifecycleEventPhase::Attempt);
    assert(initialize_attempt.state_before == EngineState::Uninitialized);
    assert(initialize_attempt.state_after == EngineState::Uninitialized);

    const auto& initialize_success = lifecycle_events[1];
    assert(initialize_success.operation == EngineLifecycleOperation::Initialize);
    assert(initialize_success.phase == EngineLifecycleEventPhase::Success);
    assert(initialize_success.state_before == EngineState::Uninitialized);
    assert(initialize_success.state_after == EngineState::Initialized);
    assert(initialize_success.error_code == 0);
    assert(initialize_success.error_message.empty());

    const auto& tick_success = lifecycle_events[3];
    assert(tick_success.operation == EngineLifecycleOperation::Tick);
    assert(tick_success.phase == EngineLifecycleEventPhase::Success);
    assert(tick_success.state_before == EngineState::Initialized);
    assert(tick_success.state_after == EngineState::Running);

    const auto& shutdown_success = lifecycle_events[5];
    assert(shutdown_success.operation == EngineLifecycleOperation::Shutdown);
    assert(shutdown_success.phase == EngineLifecycleEventPhase::Success);
    assert(shutdown_success.state_before == EngineState::Running);
    assert(shutdown_success.state_after == EngineState::Stopped);

    const auto trace_events = collect_trace_events(sink);
    assert(trace_events.size() == 1);
    assert(trace_events.front().frame_index == frame_context.frame_index);
    assert(trace_events.front().delta_time == 16000000);
    assert(trace_events.front().timestamp == 42000000000);

    assert(tracer.events.size() == 1);
    assert(tracer.events.front().frame_index == frame_context.frame_index);
    assert(tracer.events.front().delta_time == trace_events.front().delta_time);

    assert(logger.payloads.size() == 6);
    const auto& initialize_success_log = find_log_by_event(logger.payloads, "initialize.success");
    assert(find_field(initialize_success_log, "operation") == "initialize");
    assert(find_field(initialize_success_log, "phase") == "success");
    assert(find_field(initialize_success_log, "state_before") == "uninitialized");
    assert(find_field(initialize_success_log, "state_after") == "initialized");
    assert(find_field(initialize_success_log, "error_code") == "0");
    assert(find_field(initialize_success_log, "error_message").empty());

    const auto& tick_success_log = find_log_by_event(logger.payloads, "tick.success");
    assert(find_field(tick_success_log, "operation") == "tick");
    assert(find_field(tick_success_log, "state_before") == "initialized");
    assert(find_field(tick_success_log, "state_after") == "running");

    const auto& shutdown_success_log = find_log_by_event(logger.payloads, "shutdown.success");
    assert(find_field(shutdown_success_log, "operation") == "shutdown");
    assert(find_field(shutdown_success_log, "state_before") == "running");
    assert(find_field(shutdown_success_log, "state_after") == "stopped");
}

void test_disables_trace_and_logger_emission_when_not_configured() {
    SpyLogger logger {};
    SpyTracer tracer {};
    SpySink sink {};

    EngineLifecycleController controller {&logger, &tracer};
    controller.add_observability_sink(&sink);

    const auto config = EngineConfig {
        .app_name = "unit-tests-no-trace",
        .target_fps = 60,
        .enable_validation = true,
        .enable_frame_trace = false,
    };

    assert(controller.initialize(config).is_ok());

    const auto frame_context = FrameContext {
        .frame_index = 11,
        .delta_time = std::chrono::milliseconds {8},
        .timestamp = FrameContext::Clock::time_point {std::chrono::seconds {100}},
    };

    assert(controller.tick(frame_context).is_ok());

    const std::size_t logs_before_disabling = logger.payloads.size();
    controller.set_logger(nullptr);
    assert(controller.shutdown().is_ok());

    assert(logger.payloads.size() == logs_before_disabling);
    assert(tracer.events.empty());

    const auto trace_events = collect_trace_events(sink);
    assert(trace_events.empty());

    const auto lifecycle_events = collect_lifecycle_events(sink);
    assert(lifecycle_events.size() == 6);
}

}  // namespace

int main() {
    test_emits_consistent_observability_for_initialize_tick_shutdown();
    test_disables_trace_and_logger_emission_when_not_configured();
    return 0;
}
