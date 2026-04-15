/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/commands/i_command_encoder.hpp"

namespace vme::engine::gfx::commands {

/**
 * @brief Cria implementação stub de `ICommandEncoder` para validação de contrato.
 *
 * Motivo:
 * - disponibilizar uma implementação mínima e determinística para testes de API;
 * - permitir evolução incremental antes da integração com backends concretos.
 */
[[nodiscard]] std::unique_ptr<ICommandEncoder> create_command_encoder_stub();

}  // namespace vme::engine::gfx::commands
