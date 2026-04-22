/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/
#include <algorithm>
#include "engine/core/contracts/lifecycle_observer_adapter.hpp"

/*
    * @brief Implementação do adaptador de observadores de lifecycle.
    * Este arquivo contém a implementação do `LifecycleObserverAdapter`, que é responsável por conectar o pipeline de observabilidade (`IObservabilitySink`) ao sistema de observadores de ciclo de vida (`ILifecycleObserver`). O adaptador recebe envelopes canônicos do core, filtra sinais relacionados a lifecycle e notifica os observers registrados quando um evento de lifecycle é emitido. Esta implementação promove o desacoplamento entre o sistema de observabilidade e os módulos que precisam reagir a transições de lifecycle, permitindo um design mais modular e flexível.
    * A implementação deste adaptador é crucial para garantir que os eventos de lifecycle sejam entregues de forma eficiente e segura aos observers interessados, sem acoplar diretamente os módulos do engine ao pipeline de observabilidade, promovendo assim uma arquitetura mais limpa e sustentável.
    * @see LifecycleObserverAdapter
    * @see ILifecycleObserver
    * @see IObservabilitySink
    * @see EngineLifecycleEvent
    * @see EngineObservabilityEnvelope
    * @see EngineState
    * @see EngineLifecycleOperation
    * @see EngineLifecycleEventPhase
*/
namespace vme::engine::core::contracts
{
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
void LifecycleObserverAdapter::add_observer(const std::shared_ptr<ILifecycleObserver>& observer) 
noexcept {
    /*
        * A implementação deste método deve garantir a thread-safety para permitir o registro dinâmico de observers durante a execução da engine. O uso de um mutex para proteger o acesso à lista de observers é uma abordagem adequada para garantir que as operações de adição sejam atômicas e consistentes, evitando condições de corrida e garantindo a integridade dos dados.
        * O método deve adicionar o observer à lista de observers registrados, permitindo que ele receba notificações futuras de eventos de lifecycle. O adapter não é responsável por gerenciar a memória do observer; o caller deve garantir que o observer seja destruído apenas após a remoção ou que permaneça válido enquanto registrado.
        * Observers duplicados (mesmo ponteiro) podem ser registrados e receberão múltiplas notificações, o que é um comportamento permitido para flexibilidade, mas o caller deve estar ciente disso para evitar registros acidentais.
    */
    {
        std::scoped_lock lock(observers_mutex_);
        auto it = std::find_if(observers_.begin(), observers_.end(),
            [&observer](const std::weak_ptr<ILifecycleObserver>& o) {
                return !o.expired() && o.lock() == observer;
            });
        if (it == observers_.end()) {
            observers_.emplace_back(observer);
        }
    }
}

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
void LifecycleObserverAdapter::remove_observer(const std::shared_ptr<ILifecycleObserver>& observer) 
noexcept {
    /*
        * A implementação deste método deve garantir a thread-safety para permitir a remoção dinâmica de
        * observers durante a execução da engine. O uso de um mutex para proteger o acesso à lista de observers é uma abordagem adequada para garantir que as operações de remoção sejam atômicas e consistentes, evitando condições de corrida e garantindo a integridade dos dados.
        * O método deve remover o observer da lista de observers registrados, garantindo que ele não receba mais notificações futuras.
    */
   {
        std::scoped_lock lock(observers_mutex_);
        observers_.erase(std::remove_if(observers_.begin(), observers_.end(),
            [&observer](const std::weak_ptr<ILifecycleObserver>& o) {
                return o.expired() || o.lock() == observer;
            }), observers_.end());
    }
}

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
void LifecycleObserverAdapter::clear_observers() 
noexcept {
    /*
        * A implementação deste método deve garantir a thread-safety para permitir a remoção dinâmica de observers durante a execução da engine. O uso de um mutex para proteger o acesso à lista de observers é uma abordagem adequada para garantir que as operações de remoção sejam atômicas e consistentes, evitando condições de corrida e garantindo a integridade dos dados.
        * O método deve limpar a lista de observers registrados, garantindo que nenhum observer receba notificações futuras. O adapter não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados.
    */
    {
        std::scoped_lock lock(observers_mutex_);
        observers_.clear();
    }
}

/*
    * @brief Notifica os observers registrados sobre um evento de lifecycle.
    * Este método é chamado internamente pelo adapter quando um envelope contendo `EngineLifecycleEvent` é exportado. Ele filtra os envelopes para identificar eventos de lifecycle e notifica os observers registrados chamando seu método `on_lifecycle_event` com o evento extraído do envelope.
    * O método deve garantir a thread-safety para permitir notificações de eventos de lifecycle durante a execução da engine, mesmo que os observers sejam registrados ou removidos dinamicamente. O uso de um mutex para proteger o acesso à lista de observers durante a notificação é uma abordagem adequada para garantir que as notificações sejam consistentes e que os observers recebam os eventos corretamente, evitando condições de corrida e garantindo a integridade dos dados.
    * @param envelope Envelope canônico contendo o evento de lifecycle a ser notificado aos observers.
    * @see add_observer
    * @see remove_observer
    * @see clear_observers
    * @see export_signal
    * @see IObservabilitySink
    * @see ILifecycleObserver
    * @see EngineLifecycleEvent
    * @see EngineObservabilityEnvelope
    * @see EngineState
    * @see EngineLifecycleOperation
    * @see EngineLifecycleEventPhase
*/
void LifecycleObserverAdapter::export_signal(const types::EngineObservabilityEnvelope& envelope) {
    /*
        * A implementação deste método deve garantir a thread-safety para permitir notificações de eventos de lifecycle durante a execução da engine, mesmo que os observers sejam registrados ou removidos dinamicamente. O uso de um mutex para proteger o acesso à lista de observers durante a notificação é uma abordagem adequada para garantir que as notificações sejam consistentes e que os observers recebam os eventos corretamente, evitando condições de corrida e garantindo a integridade dos dados.
        * O método deve filtrar os envelopes para identificar eventos de lifecycle, verificando se o sinal contido no envelope é do tipo `EngineLifecycleEvent`. Se o sinal for do tipo correto, o método deve extrair o evento do envelope e notificar os observers registrados chamando seu método `on_lifecycle_event` com o evento extraído.
        * O adapter não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados. O método deve lidar corretamente com observers expirados (usando `std::weak_ptr`) para evitar notificações para observers inválidos, garantindo a segurança da memória e a robustez do sistema.
    */
    if (!std::holds_alternative<types::EngineLifecycleEvent>(envelope.signal)) {
        return;
    }
    // Extrai o evento de lifecycle do envelope e notifica os observers registrados. O uso de std::get é seguro aqui porque já verificamos o tipo do sinal com std::holds_alternative. O método deve garantir que os observers recebam o evento corretamente, mesmo que a lista de observers seja modificada durante a notificação, evitando condições de corrida e garantindo a integridade dos dados.
    const auto& event = std::get<types::EngineLifecycleEvent>(envelope.signal);

    // Cria uma cópia da lista de observers ativos para notificar, evitando problemas de concorrência se a lista for modificada durante a notificação. O uso de std::weak_ptr permite que o adapter detecte quando um observer foi destruído e evite notificações para observers inválidos, garantindo a segurança da memória e a robustez do sistema. O método deve garantir que os observers recebam o evento corretamente, mesmo que a lista de observers seja modificada durante a notificação, evitando condições de corrida e garantindo a integridade dos dados.
    std::vector<std::shared_ptr<ILifecycleObserver>> active_observers;
    {
        // Lock para acessar a lista de observers e criar uma cópia dos observers ativos, removendo os expirados. O uso de std::scoped_lock garante que o mutex seja liberado automaticamente ao final do escopo, mesmo em caso de exceções, garantindo a segurança da memória e a robustez do sistema. O método deve garantir que os observers recebam o evento corretamente, mesmo que a lista de observers seja modificada durante a notificação, evitando condições de corrida e garantindo a integridade dos dados.
        std::scoped_lock lock(observers_mutex_);
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&active_observers](const std::weak_ptr<ILifecycleObserver>& wp) {
                    if (auto obs = wp.lock()) {
                        active_observers.push_back(std::move(obs));
                        return false;
                    }
                    return true;
                }),
            observers_.end());
    }

    // Notifica os observers ativos sobre o evento de lifecycle. O método deve garantir que os observers recebam o evento corretamente, mesmo que a lista de observers seja modificada durante a notificação, evitando condições de corrida e garantindo a integridade dos dados. O adapter não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados. O método deve lidar corretamente com observers expirados (usando std::weak_ptr) para evitar notificações para observers inválidos, garantindo a segurança da memória e a robustez do sistema.
    for (const auto& obs : active_observers) {
        // O método on_lifecycle_event deve ser chamado para cada observer ativo, passando o evento de lifecycle extraído do envelope. O método deve garantir que os observers recebam o evento corretamente, mesmo que a lista de observers seja modificada durante a notificação, evitando condições de corrida e garantindo a integridade dos dados. O adapter não é responsável por gerenciar a memória dos observers; o caller deve garantir que os observers sejam destruídos apenas após a remoção ou que permaneçam válidos enquanto registrados. O método deve lidar corretamente com observers expirados (usando std::weak_ptr) para evitar notificações para observers inválidos, garantindo a segurança da memória e a robustez do sistema.
        obs->on_lifecycle_event(event);
    }
}



} // namespace vme::engine::core::contracts

