/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// cstdint para códigos numéricos estáveis de integração host.
#include <cstdint>

/*
    * @brief Catálogo estável de erros do lifecycle.
    *
    * Convenções:
    * - nomes semânticos para evitar uso de "números mágicos";
    * - códigos e mensagens são contratos de integração e não devem mudar
    *   sem controle de versionamento do host;
    * - `Lifecycle*` indica erro originado na máquina de lifecycle.
    * Códigos de erro e mensagens associadas para operações de lifecycle, permitindo diagnóstico detalhado de problemas durante as transições de lifecycle. Estes códigos e mensagens são contratos de integração e devem ser estáveis para garantir a utilidade dos eventos de lifecycle para diagnóstico e análise de problemas.
    * Os códigos de erro são números inteiros que representam a natureza do erro, enquanto as
    * mensagens de erro são strings que fornecem detalhes adicionais sobre o erro. Ambos os campos são opcionais e devem ser preenchidos apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
    * @see EngineLifecycleOperation
    * @see EngineLifecycleEventPhase
    * @see EngineLifecycleEvent
    * @see vme::engine::core::contracts::LifecycleObserverAdapter
    * @see vme::engine::core::contracts::ILifecycleObserver
    * @see vme::engine::core::contracts::IObservabilitySink
*/
namespace vme::engine::core::types {

/**
 * @brief Catálogo estável de erros do lifecycle.
 *
 * Convenções:
 * - nomes semânticos para evitar uso de "números mágicos";
 * - códigos e mensagens são contratos de integração e não devem mudar
 *   sem controle de versionamento do host;
 * - `Lifecycle*` indica erro originado na máquina de lifecycle.
 */
namespace lifecycle_error {

// Códigos de erro e mensagens associadas para operações de lifecycle, permitindo diagnóstico detalhado de problemas durante as transições de lifecycle. Estes códigos e mensagens são contratos de integração e devem ser estáveis para garantir a utilidade dos eventos de lifecycle para diagnóstico e análise de problemas.
// Os códigos de erro são números inteiros que representam a natureza do erro, enquanto as mensagens de erro são strings que fornecem detalhes adicionais sobre o erro. Ambos os campos são opcionais e devem ser preenchidos apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
inline constexpr std::uint32_t LifecycleInitializeInvalidState = 1001U;
inline constexpr char LifecycleInitializeInvalidStateMessage[] = "initialize requer estado Uninitialized.";

inline constexpr std::uint32_t LifecycleInvalidConfig = 1002U;
inline constexpr char LifecycleInvalidConfigMessage[] = "EngineConfig inválido: app_name vazio ou target_fps <= 0.";

inline constexpr std::uint32_t LifecycleTickInvalidState = 1003U;
inline constexpr char LifecycleTickInvalidStateMessage[] = "tick requer estado Running ou Initialized.";

// Códigos de erro e mensagens associadas para operações de lifecycle, permitindo diagnóstico detalhado de problemas durante as transições de lifecycle. Estes códigos e mensagens são contratos de integração e devem ser estáveis para garantir a utilidade dos eventos de lifecycle para diagnóstico e análise de problemas.
// Os códigos de erro são números inteiros que representam a natureza do erro, enquanto as mensagens de erro são strings que fornecem detalhes adicionais sobre o erro. Ambos os campos são opcionais e devem ser preenchidos apenas para eventos de falha (Failure) para garantir a utilidade do evento para diagnóstico e análise de problemas de lifecycle.
inline constexpr std::uint32_t LifecycleInvalidFrameContext = 1004U;
inline constexpr char LifecycleInvalidFrameContextMessage[] = "FrameContext inválido: delta_time negativo.";

inline constexpr std::uint32_t LifecycleShutdownInvalidState = 1005U;
inline constexpr char LifecycleShutdownInvalidStateMessage[] = "shutdown requer engine inicializada.";

inline constexpr std::uint32_t LifecyclePauseInvalidState = 1006U;
inline constexpr char LifecyclePauseInvalidStateMessage[] = "pause requer estado Running.";

inline constexpr std::uint32_t LifecycleResumeInvalidState = 1007U;
inline constexpr char LifecycleResumeInvalidStateMessage[] = "resume requer estado Paused.";

} // namespace lifecycle_error

} // namespace vme::engine::core::types
