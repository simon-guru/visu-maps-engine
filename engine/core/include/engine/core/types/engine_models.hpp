/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// chrono: representa tempo de frame sem depender de backend específico.
#include <chrono>
// cstdint: padroniza tamanhos de tipos numéricos usados pela API pública.
#include <cstdint>
// string/string_view: mensagens de erro e serialização textual leve.
#include <string>
#include <string_view>

namespace vme::engine::core::types {

/**
 * @brief Estados globais da engine.
 *
 * Motivo da abordagem enum class:
 * - evita colisão de nomes no escopo global;
 * - reforça tipagem forte nas transições de lifecycle.
 */
enum class EngineState : std::uint8_t {
    // Estado inicial antes de qualquer bootstrap.
    Uninitialized = 0,
    // Bootstrap concluído, pronto para primeiro tick.
    Initialized,
    // Execução de frames em andamento.
    Running,
    // Execução temporariamente pausada.
    Paused,
    // Encerramento em progresso.
    Stopping,
    // Runtime finalizado.
    Stopped,
};

/**
 * @brief Nível semântico para classificação de erro.
 */
enum class EngineErrorSeverity : std::uint8_t {
    // Informação não crítica.
    Info = 0,
    // Situação não ideal, porém tolerável.
    Warning,
    // Erro recuperável com tratamento local.
    Recoverable,
    // Erro crítico que impede continuidade segura.
    Fatal,
};

/**
 * @brief Estrutura padrão para transportar erros do core.
 */
struct EngineError final {
    // Código numérico para automação, telemetria e troubleshooting.
    std::uint32_t code {0};
    // Gravidade associada ao erro.
    EngineErrorSeverity severity {EngineErrorSeverity::Info};
    // Mensagem legível para logs e diagnóstico humano.
    std::string message {};

    // Convenção: code == 0 representa operação bem-sucedida.
    [[nodiscard]] constexpr bool is_ok() const noexcept { return code == 0U; }
};

/**
 * @brief Configuração mínima do runtime.
 */
struct EngineConfig final {
    // Identificador amigável da aplicação host.
    std::string app_name {"visu-maps-engine"};
    // Meta de FPS para loop principal.
    std::uint32_t target_fps {60U};
    // Ativa validações extras para modo desenvolvimento.
    bool enable_validation {true};
    // Ativa tracing por frame para observabilidade.
    bool enable_frame_trace {false};

    // Validação local para impedir bootstrap com parâmetros inviáveis.
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return !app_name.empty() && target_fps > 0U;
    }
};

/**
 * @brief Contexto temporal de cada frame processado.
 */
struct FrameContext final {
    // Clock monotônico apropriado para medições de intervalo.
    using Clock = std::chrono::steady_clock;

    // Índice incremental do frame.
    std::uint64_t frame_index {0U};
    // Delta de tempo desde o frame anterior.
    Clock::duration delta_time {Clock::duration::zero()};
    // Timestamp de referência para o frame atual.
    Clock::time_point timestamp {Clock::now()};
};

/**
 * @brief Converte estado de lifecycle para string estável de diagnóstico.
 */
[[nodiscard]] std::string_view to_string(EngineState state) noexcept;

/**
 * @brief Converte severidade de erro para string estável de diagnóstico.
 */
[[nodiscard]] std::string_view to_string(EngineErrorSeverity severity) noexcept;

}  // namespace vme::engine::core::types
