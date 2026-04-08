/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/contracts/i_observability_sink.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Sink padrão no-op para ambientes sem backend de observabilidade.
 */
class NoopObservabilitySink final : public IObservabilitySink {
public:
    ~NoopObservabilitySink() override = default;

    void export_signal(const types::EngineObservabilityEnvelope&) override {}
};

}  // namespace vme::engine::core::contracts
