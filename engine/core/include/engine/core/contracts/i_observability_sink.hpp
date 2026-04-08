/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/types/engine_observability.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato de exportação de observabilidade desacoplado de backend.
 */
class IObservabilitySink {
public:
    virtual ~IObservabilitySink() = default;

    /**
     * @brief Exporta envelope canônico para um sink externo.
     */
    virtual void export_signal(const types::EngineObservabilityEnvelope& envelope) = 0;
};

}  // namespace vme::engine::core::contracts
