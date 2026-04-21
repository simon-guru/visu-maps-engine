/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <algorithm>
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

    void add_observer(ILifecycleObserver* observer) noexcept {
        if (observer == nullptr) {
            return;
        }

        std::lock_guard<std::mutex> lock {mutex_};
        const auto it = std::find(observers_.begin(), observers_.end(), observer);
        if (it == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    void remove_observer(ILifecycleObserver* observer) noexcept {
        std::lock_guard<std::mutex> lock {mutex_};
        const auto begin_it = observers_.begin();
        const auto end_it = observers_.end();
        const auto removed_begin = std::remove(begin_it, end_it, observer);
        observers_.erase(removed_begin, end_it);
    }

    void clear_observers() noexcept {
        std::lock_guard<std::mutex> lock {mutex_};
        observers_.clear();
    }

    void export_signal(const types::EngineObservabilityEnvelope& envelope) override {
        if (!std::holds_alternative<types::EngineLifecycleEvent>(envelope.signal)) {
            return;
        }

        std::vector<ILifecycleObserver*> snapshot {};
        {
            std::lock_guard<std::mutex> lock {mutex_};
            snapshot = observers_;
        }

        const auto& event = std::get<types::EngineLifecycleEvent>(envelope.signal);
        for (auto* observer : snapshot) {
            if (observer != nullptr) {
                observer->on_lifecycle_event(event);
            }
        }
    }

private:
    mutable std::mutex mutex_ {};
    std::vector<ILifecycleObserver*> observers_ {};
};

}  // namespace vme::engine::core::contracts
