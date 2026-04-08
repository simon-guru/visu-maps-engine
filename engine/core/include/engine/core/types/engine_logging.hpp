/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <string>
#include <vector>

#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::types {

/**
 * @brief Campo simples de log estruturado no formato chave-valor.
 */
struct EngineLogField final {
    std::string key {};
    std::string value {};
};

/**
 * @brief Payload canônico para eventos de observabilidade do core.
 */
struct EngineLogPayload final {
    // Módulo emissor do evento (ex.: "engine.core.lifecycle").
    std::string module {"engine.core"};
    // Severidade do evento; reutiliza tipagem já existente no core.
    EngineErrorSeverity severity {EngineErrorSeverity::Info};
    // Nome curto da operação/evento (ex.: "initialize", "tick", "shutdown").
    std::string event {};
    // Mensagem legível para troubleshooting humano.
    std::string message {};
    // Metadados adicionais com diagnóstico estruturado.
    std::vector<EngineLogField> fields {};
};

}  // namespace vme::engine::core::types
