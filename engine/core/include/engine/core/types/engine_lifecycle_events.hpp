/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "engine/core/types/engine_models.hpp"
/*
    * @brief Tipos relacionados a eventos de lifecycle da engine.
    * Estes tipos são fundamentais para a implementação do sistema de observabilidade e para a emissão de eventos de lifecycle que permitem monitorar e diagnosticar o comportamento da engine durante suas transições de estado.
    * A definição clara destes tipos é crucial para garantir a consistência e a utilidade dos eventos de lifecycle, permitindo que os módulos do engine e os sistemas de observabilidade possam reagir de forma adequada às transições de lifecycle e fornecer insights valiosos sobre o comportamento da engine.
    * @see EngineLifecycleOperation
    * @see EngineLifecycleEventPhase
    * @see EngineLifecycleEvent
*/
namespace vme::engine::core::types {

/**
 * @brief Operações suportadas pela máquina de lifecycle.
 * Cada operação representa uma transição ou ação significativa no ciclo de vida da engine, como inicialização, execução de um tick, pausa, retomada e desligamento. Estas operações são fundamentais para a gestão do estado da engine e para a emissão de eventos de lifecycle que permitem observabilidade e diagnóstico do comportamento da engine durante sua execução.
 * A definição clara destas operações é crucial para garantir a consistência e a utilidade dos eventos de lifecycle, permitindo que os módulos do engine e os sistemas de observabilidade possam reagir de forma adequada às transições de lifecycle e fornecer insights valiosos sobre o comportamento da engine.
 */
enum class EngineLifecycleOperation : std::uint8_t {
    Initialize = 0, // Operação de inicialização da engine, incluindo configuração, alocação de recursos e preparação para execução.
    Tick, // Operação de execução de um ciclo de atualização da engine, incluindo processamento de lógica, renderização e outras tarefas relacionadas ao ciclo de vida durante a execução.
    Pause, // Operação de pausa da engine, suspendendo temporariamente a execução e preservando o estado atual para retomada posterior.
    Resume, // Operação de retomada da engine após uma pausa, restaurando o estado e reiniciando a execução normal.
    Shutdown, // Operação de desligamento da engine, incluindo liberação de recursos, salvamento de estado e preparação para encerramento completo.
};

/**
 * @brief Fase do evento emitido para cada operação de lifecycle.
 */
enum class EngineLifecycleEventPhase : std::uint8_t {
    Attempt = 0,// Indica o início de uma operação de lifecycle, antes de sua conclusão. Esta fase é útil para medir a duração total da operação e para identificar quando as operações estão sendo iniciadas, mesmo que ainda não tenham sido concluídas.
    Success, // Indica a conclusão bem-sucedida de uma operação de lifecycle. Esta fase é útil para medir a duração de operações bem-sucedidas e para identificar quando as operações estão sendo concluídas com sucesso.
    Failure, // Indica a conclusão de uma operação de lifecycle com falha. Esta fase é útil para medir a duração de operações que falharam e para identificar quando as operações estão sendo concluídas com falha, permitindo análise de erros e diagnóstico.
};

/**
 * @brief Evento canônico de observabilidade para operações de lifecycle.
 */
struct EngineLifecycleEvent final {
    // Operação de lifecycle associada ao evento.
    // A operação é definida pelo contexto de emissão do evento, como inicialização, tick, pausa, etc.
    // A operação é um indicador chave para análise de telemetria e diagnóstico, permitindo categorizar os eventos de lifecycle e entender o comportamento da engine em diferentes fases de execução.
    // A operação é um campo obrigatório e deve ser preenchida corretamente para garantir a utilidade do evento para observabilidade e análise.
    // Fase do evento, indicando se é uma tentativa, sucesso ou falha da operação de lifecycle.
    EngineLifecycleOperation operation {EngineLifecycleOperation::Initialize};

    // Fase do evento, indicando se é uma tentativa, sucesso ou falha da operação de lifecycle.
    // A fase é um indicador crítico para análise de telemetria, permitindo diferenciar entre
    // eventos que representam tentativas de operações (Attempt), operações bem-sucedidas (Success) e falhas (Failure).
    // A fase é um campo obrigatório e deve ser preenchida corretamente para garantir a utilidade
    EngineLifecycleEventPhase phase {EngineLifecycleEventPhase::Attempt};

    // Estados da engine antes e depois da operação, fornecendo contexto adicional para análise de transições de lifecycle.
    // Os estados são definidos pelo enum EngineState e devem refletir com precisão o estado da
    // engine antes e depois da operação para garantir a utilidade do evento para diagnóstico e análise de comportamento.
    EngineState state_before {EngineState::Uninitialized};

    // Estado da engine após a operação, fornecendo contexto adicional para análise de transições de lifecycle.
    // O estado após a operação é um indicador importante para entender o resultado da operação e o
    // comportamento da engine, especialmente em casos de falha, onde o estado após a operação pode indicar um estado de erro ou recuperação.
    EngineState state_after {EngineState::Uninitialized};

    // Código de erro e mensagem associada para eventos de falha, permitindo diagnóstico detalhado de problemas de lifecycle.
    // O código de erro é um número inteiro que representa a natureza do erro, enquanto a
    // mensagem de erro é uma string que fornece detalhes adicionais sobre o erro. Ambos os campos são opcionais e devem ser preenchidos apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
    // O código de erro é um número inteiro que representa a natureza do erro, enquanto a mensagem de erro é uma string que fornece detalhes adicionais sobre o erro. Ambos os campos são opcionais e devem ser preenchidos apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
    std::uint32_t error_code {0U};

    // Mensagem de erro associada para eventos de falha, permitindo diagnóstico detalhado de problemas de lifecycle.
    // A mensagem de erro é uma string que fornece detalhes adicionais sobre o erro. Este campo é opcional e deve ser preenchido apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
    std::string error_message {};
};

/**
 * @brief Converte operação de lifecycle para string estável de telemetria.
 * A função to_string é uma função constexpr que converte um valor do enum EngineLifecycleOperation para uma string_view correspondente. A função é marcada como [[nodiscard]] para indicar que o valor de retorno deve ser utilizado, e é noexcept para indicar que não lança exceções. A função utiliza um switch para mapear cada valor do enum para uma string específica, e retorna "unknown" para valores não reconhecidos. Esta função é útil para gerar strings legíveis a partir dos valores do enum, especialmente em contextos de telemetria e logging onde a legibilidade é importante.
 */
[[nodiscard]] constexpr std::string_view to_string(const EngineLifecycleOperation operation) 
noexcept {
    switch (operation) {
        case EngineLifecycleOperation::Initialize: // Operação de inicialização da engine, incluindo configuração, alocação de recursos e preparação para execução.
            return "initialize";
        case EngineLifecycleOperation::Tick: // Operação de execução de um ciclo de atualização da engine, incluindo processamento de lógica, renderização e outras tarefas relacionadas ao ciclo de vida durante a execução.
            return "tick";
        case EngineLifecycleOperation::Pause: // Operação de pausa da engine, suspendendo temporariamente a execução e preservando o estado atual para retomada posterior.
            return "pause";
        case EngineLifecycleOperation::Resume: // Operação de retomada da engine após uma pausa, restaurando o estado e reiniciando a execução normal.
            return "resume";
        case EngineLifecycleOperation::Shutdown: // Operação de desligamento da engine, incluindo liberação de recursos, salvamento de estado e preparação para encerramento completo.
            return "shutdown";
    }
    return "unknown";
}

/**
 * @brief Converte fase do evento para string estável de telemetria.
 * A função to_string é uma função constexpr que converte um valor do enum EngineLifecycleEventPhase para uma string_view correspondente. A função é marcada como [[nodiscard]] para indicar que o valor de retorno deve ser utilizado, e é noexcept para indicar que não lança exceções. A função utiliza um switch para mapear cada valor do enum para uma string específica, e retorna "unknown" para valores não reconhecidos. Esta função é útil para gerar strings legíveis a partir dos valores do enum, especialmente em contextos de telemetria e logging onde a legibilidade é importante.
 * A fase do evento é um indicador crítico para análise de telemetria, permitindo diferenciar entre eventos que representam tentativas de operações (Attempt), operações bem-sucedidas (Success) e falhas (Failure). A conversão para string é especialmente útil para gerar logs e métricas legíveis, facilitando a análise e o diagnóstico do comportamento da engine durante suas transições de lifecycle.
 */
[[nodiscard]] constexpr std::string_view to_string(const EngineLifecycleEventPhase phase)
noexcept {
    switch (phase) {
        case EngineLifecycleEventPhase::Attempt: // Indica o início de uma operação de lifecycle, antes de sua conclusão. Esta fase é útil para medir a duração total da operação e para identificar quando as operações estão sendo iniciadas, mesmo que ainda não tenham sido concluídas.
            return "attempt";
        case EngineLifecycleEventPhase::Success: // Indica a conclusão bem-sucedida de uma operação de lifecycle. Esta fase é útil para medir a duração de operações bem-sucedidas e para identificar quando as operações estão sendo concluídas com sucesso.
            return "success";
        case EngineLifecycleEventPhase::Failure: // Indica a conclusão de uma operação de lifecycle com falha. Esta fase é útil para medir a duração de operações que falharam e para identificar quando as operações estão sendo concluídas com falha, permitindo análise de erros e diagnóstico.
            return "failure";
    }

    return "unknown";
}

}  // namespace vme::engine::core::types
