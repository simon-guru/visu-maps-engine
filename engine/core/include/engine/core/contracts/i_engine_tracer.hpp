/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/core/types/engine_trace_events.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato de tracing estruturado para eventos temporais do runtime.
 *
 * Mantido separado de logger para evitar acoplamento entre telemetria de frame
 * e logging operacional geral.
 */
class IEngineTracer {
public:
    virtual ~IEngineTracer() = default;

    /**
     * @brief Emite evento de trace por frame já normalizado para exportação.
     */
    virtual void trace_frame(const types::FrameTraceEvent& event) = 0;
};

}  // namespace vme::engine::core::contracts
