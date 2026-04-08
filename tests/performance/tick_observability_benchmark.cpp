#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

namespace {
using vme::engine::core::lifecycle::EngineLifecycleController;
using vme::engine::core::types::EngineConfig;
using vme::engine::core::types::FrameContext;

constexpr std::size_t kDefaultIterations = 200000;

std::int64_t benchmark_tick_ns(const bool enable_frame_trace, const std::size_t iterations) {
    EngineLifecycleController controller {};

    const auto config = EngineConfig {
        .app_name = "performance-benchmark",
        .target_fps = 60,
        .enable_validation = false,
        .enable_frame_trace = enable_frame_trace,
    };

    if (!controller.initialize(config).is_ok()) {
        return -1;
    }

    const auto begin = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        const auto frame = FrameContext {
            .frame_index = static_cast<std::uint64_t>(i),
            .delta_time = std::chrono::milliseconds {16},
            .timestamp = FrameContext::Clock::time_point {std::chrono::milliseconds {16 * i}},
        };

        if (!controller.tick(frame).is_ok()) {
            return -1;
        }
    }
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
}

}  // namespace

int main(int argc, char** argv) {
    std::size_t iterations = kDefaultIterations;
    if (argc > 1) {
        iterations = static_cast<std::size_t>(std::strtoull(argv[1], nullptr, 10));
        if (iterations == 0) {
            std::cerr << "iterations must be > 0\n";
            return 2;
        }
    }

    const auto off_ns = benchmark_tick_ns(false, iterations);
    const auto on_ns = benchmark_tick_ns(true, iterations);

    if (off_ns <= 0 || on_ns <= 0) {
        std::cerr << "benchmark setup failed\n";
        return 3;
    }

    const double off_per_tick = static_cast<double>(off_ns) / static_cast<double>(iterations);
    const double on_per_tick = static_cast<double>(on_ns) / static_cast<double>(iterations);
    const double overhead_ratio = on_per_tick / off_per_tick;

    std::cout << "tick benchmark iterations=" << iterations << '\n';
    std::cout << "tick off avg(ns): " << off_per_tick << '\n';
    std::cout << "tick on avg(ns): " << on_per_tick << '\n';
    std::cout << "overhead ratio (on/off): " << overhead_ratio << '\n';

    constexpr double kAcceptedOverheadRatio = 1.5;
    if (overhead_ratio > kAcceptedOverheadRatio) {
        std::cerr << "overhead gate failed: ratio=" << overhead_ratio
                  << " > " << kAcceptedOverheadRatio << '\n';
        return 4;
    }

    return 0;
}
