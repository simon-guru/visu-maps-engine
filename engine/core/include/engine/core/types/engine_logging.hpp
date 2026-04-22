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
 * O EngineLogPayload é um tipo de dado que representa a estrutura de um evento de log emitido pelo core da engine. Ele contém informações essenciais para o diagnóstico e análise do comportamento da engine, incluindo o módulo emissor, a severidade do evento, o nome do evento, uma mensagem legível e campos adicionais para metadados estruturados. Este tipo é fundamental para a implementação do sistema de observabilidade e para a emissão de eventos de log que permitem monitorar e diagnosticar o comportamento da engine durante sua execução.
 * A definição clara deste tipo é crucial para garantir a consistência e a utilidade dos eventos
 * de log, permitindo que os módulos do engine e os sistemas de observabilidade possam reagir de forma adequada aos eventos de log e fornecer insights valiosos sobre o comportamento da engine.
 * @see EngineObservabilitySignal
 * @see EngineObservabilityEnvelope
 * @see IObservabilitySink
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
