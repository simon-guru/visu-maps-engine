/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/types/engine_lifecycle_events.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Observador especializado para eventos de lifecycle.
 *
 * Objetivo:
 * - permitir consumo reativo dos eventos de lifecycle sem duplicar o
 *   pipeline canônico de observabilidade (`IObservabilitySink`).
 *
 * Uso recomendado:
 * - módulos que precisam reagir a transições (`render`, `gfx`, `tiles`, `services`)
 *   implementam este contrato;
 * - um adapter de observabilidade traduz `EngineObservabilityEnvelope`
 *   para chamadas deste observer.
 */
class ILifecycleObserver {
public:
    virtual ~ILifecycleObserver() = default;

    /**
     * @brief Recebe evento de lifecycle já filtrado e tipado.
     * O método é chamado pelo adapter de observabilidade quando um envelope contendo `EngineLifecycleEvent` é exportado.
     * O caller deve garantir que a implementação deste método seja thread-safe, pois pode ser chamada de múltiplas threads dependendo do contexto de exportação dos envelopes.
     * @param event Evento de lifecycle contendo informações sobre a operação, fase, estados e erros relacionados à transição de lifecycle.
      * @see EngineLifecycleEvent
      * @see LifecycleObserverAdapter
      * @see IObservabilitySink
     */
    virtual void on_lifecycle_event(const types::EngineLifecycleEvent& event) = 0;
};

}  // namespace vme::engine::core::contracts
