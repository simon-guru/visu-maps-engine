/*
Copyright (c) 2026. VISU LLC. All rights reserved.
Author: Simão A.Mayunga
Date: 21/04/2026
This file is part of VISU LLC.

Este arquivo introduz um adaptador que conecta o pipeline de observabilidade (IObservabilitySink) 
ao sistema de observadores de ciclo de vida (ILifecycleObserver). 
É uma peça de engenharia inteligente que promove o desacoplamento.
*/

#pragma once
#include <memory>
#include <mutex>
#include <vector>

#include "engine/core/contracts/i_lifecycle_observer.hpp"
#include "engine/core/contracts/i_observability_sink.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Adapter que especializa o pipeline `IObservabilitySink` para lifecycle.
 *
 * Comportamento:
 * - recebe envelopes canônicos do core;
 * - ignora sinais não-lifecycle (`log` e `trace`);
 * - notifica observers registrados quando o sinal contém `EngineLifecycleEvent`.
 */
class LifecycleObserverAdapter final : public IObservabilitySink {
public:
    LifecycleObserverAdapter() = default;
    ~LifecycleObserverAdapter() override = default;

    /*
        * @brief Registra um observer para receber eventos de lifecycle.
        * @param observer Ponteiro para o observer a ser registrado.
        * O adapter mantém uma cópia do ponteiro, mas não é responsável por sua gestão de memória.
        * O caller deve garantir que o observer permaneça válido enquanto registrado.
        * Observers duplicados (mesmo ponteiro) podem ser registrados e receberão múltiplas notificações.
        * @note O método é thread-safe para permitir registro dinâmico durante a execução da       
        * engine, mas recomenda-se registrar observers durante a fase de inicialização para evitar condições de corrida ou notificações perdidas.
        * @see remove_observer
        * @see clear_observers
        * @see on_lifecycle_event
        * @see export_signal
        * @see IObservabilitySink
        * @see ILifecycleObserver
        * @see EngineLifecycleEvent
        * @see EngineObservabilityEnvelope  
    */
    void add_observer(const std::shared_ptr<ILifecycleObserver>& observer) noexcept;

    /*
        * @brief Remove um observer previamente registrado.
        * @param observer Ponteiro para o observer a ser removido.
        * O método é thread-safe e pode ser chamado a qualquer momento. 
        * Se o observer não estiver registrado, a chamada é silenciosa (sem efeito).
        * Observers removidos não receberão mais notificações, mesmo que ainda existam referências a eles fora do adapter.
        * @note O método não é responsável por gerenciar a memória do observer; o caller deve garantir que o observer seja destruído apenas após a remoção.
        * @warning Se um observer for removido enquanto uma notificação está em andamento, ele pode receber a notificação atual, mas não as futuras. O comportamento é definido para evitar condições de corrida e garantir a consistência do estado interno do adapter.
        * @note Para evitar problemas de concorrência, recomenda-se que os observers sejam removidos antes de serem destruídos, especialmente se a notificação de eventos for frequente ou ocorrer em múltiplas threads.
        * @see add_observer
        * @see clear_observers
        * @see on_lifecycle_event
        * @see export_signal
        * @see IObservabilitySink
        * @see ILifecycleObserver
        * @see EngineLifecycleEvent
        * @see EngineObservabilityEnvelope
        * @see EngineState
        * @see EngineLifecycleOperation
        * @see EngineLifecycleEventPhase
    */
    void remove_observer(const std::shared_ptr<ILifecycleObserver>& observer) noexcept;

    /*
        * @brief Remove todos os observers registrados.
        * O método é thread-safe e pode ser chamado a qualquer momento. 
        * Após a chamada, nenhum observer receberá notificações futuras, mesmo que ainda existam referências a eles fora do adapter.
        * @note O método não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção.
        * @warning Se os observers forem removidos enquanto uma notificação está em andamento, eles podem receber a notificação atual, mas não as futuras. O comportamento é definido para evitar condições de corrida e garantir a consistência do estado interno do adapter.
        * @note Para evitar problemas de concorrência, recomenda-se que os observers sejam removidos antes de serem destruídos, especialmente se a notificação de eventos for frequente ou ocorrer em múltiplas threads.
        * @see add_observer
        * @see remove_observer
        * @see on_lifecycle_event
        * @see export_signal
        * @see IObservabilitySink
        * @see ILifecycleObserver
        * @see EngineLifecycleEvent
        * @see EngineObservabilityEnvelope
        * @see EngineState
        * @see EngineLifecycleOperation
        * @see EngineLifecycleEventPhase
    */
    void clear_observers() noexcept;

    /*
        * @brief Exporta um sinal para todos os observers registrados.
        * @param envelope Envelope contendo o sinal a ser exportado.
        * @note O método é thread-safe e pode ser chamado a qualquer momento.
        * @see add_observer
        * @see remove_observer
        * @see clear_observers
        * @see on_lifecycle_event
    */
    void export_signal(const types::EngineObservabilityEnvelope& envelope) override;

private:
    // Mutex para proteger o acesso à lista de observers, garantindo a thread-safety durante adição, remoção e notificação de observers.
    // O uso de std::mutex é adequado para proteger seções críticas de código que modificam ou acessam a lista de observers, evitando condições de corrida e garantindo a integridade dos dados.
    // O mutex é utilizado para sincronizar o acesso à lista de observers durante as operações de adição, remoção e notificação, garantindo que as operações sejam atômicas e consistentes mesmo em ambientes multithread.
    mutable std::mutex observers_mutex_ {};
    // Observers registrados para receber notificações de lifecycle. O adapter mantém referências fracas para evitar ciclos de referência e permitir a destruição dos observers sem necessidade
    // de remoção explícita. O caller deve garantir que os observers permaneçam válidos enquanto registrados.
    // O uso de std::weak_ptr permite que o adapter detecte quando um observer foi destruído e evite notificações para observers inválidos, garantindo a segurança da memória e a robustez do sistema.
    // O adapter não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados.
    std::vector<std::weak_ptr<ILifecycleObserver>> observers_ {};
};

}  // namespace vme::engine::core::contracts
