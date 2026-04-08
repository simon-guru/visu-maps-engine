/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/types/engine_logging.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato de logging estruturado para eventos do runtime.
 */
class IEngineLogger {
public:
    virtual ~IEngineLogger() = default;

    /**
     * @brief Registra um evento estruturado contendo metadados de diagnóstico.
     */
    virtual void log(const types::EngineLogPayload& payload) = 0;
};

}  // namespace vme::engine::core::contracts
