#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace visu::engine::core::types {

/**
 * @brief Representa os estados globais possíveis da engine.
 *
 * O estado é atualizado pelo lifecycle e consumido por módulos interessados
 * em sincronizar inicialização, execução de frame e desligamento.
 */
enum class EngineState : std::uint8_t {
    Uninitialized = 0,
    Initialized,
    Running,
    Paused,
    Stopping,
    Stopped,
};

/**
 * @brief Severidade semântica para erros emitidos pelo core.
 */
enum class EngineErrorSeverity : std::uint8_t {
    Info = 0,
    Warning,
    Recoverable,
    Fatal,
};

/**
 * @brief Modelo base para erros de domínio do engine-core.
 */
struct EngineError final {
    std::uint32_t code {0};
    EngineErrorSeverity severity {EngineErrorSeverity::Info};
    std::string message {};

    [[nodiscard]] constexpr bool is_ok() const noexcept { return code == 0U; }
};

/**
 * @brief Configuração mínima para bootstrap e execução da engine.
 */
struct EngineConfig final {
    std::string app_name {"visu-maps-engine"};
    std::uint32_t target_fps {60U};
    bool enable_validation {true};
    bool enable_frame_trace {false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return !app_name.empty() && target_fps > 0U;
    }
};

/**
 * @brief Metadados de um frame processado no tick da engine.
 */
struct FrameContext final {
    using Clock = std::chrono::steady_clock;

    std::uint64_t frame_index {0U};
    Clock::duration delta_time {Clock::duration::zero()};
    Clock::time_point timestamp {Clock::now()};
};

/**
 * @brief Converte EngineState para texto estável para logs e diagnósticos.
 */
[[nodiscard]] std::string_view to_string(EngineState state) noexcept;

/**
 * @brief Converte EngineErrorSeverity para texto estável para logs e diagnósticos.
 */
[[nodiscard]] std::string_view to_string(EngineErrorSeverity severity) noexcept;

}  // namespace visu::engine::core::types
