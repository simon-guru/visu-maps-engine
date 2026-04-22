/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/contracts/i_gfx_device.hpp"
#include "engine/gfx/contracts/i_gfx_instance.hpp"
#include "engine/gfx/contracts/i_gfx_queue.hpp"

/*
    
*/
namespace vme::engine::gfx::contracts {
//  Decisão de design:
// - a fábrica é responsável por criar instâncias, dispositivos e filas gráficas, encapsulando a lógica de criação e configuração desses objetos.
// - a fábrica pode ser implementada por diferentes backends gráficos, permitindo flexibilidade e abstração na criação de recursos gráficos.
// - a fábrica pode incluir métodos auxiliares para criar objetos de teste ou stubs, facilitando o desenvolvimento e testes do sistema gráfico sem depender de implementações específicas de backends.
[[nodiscard]] std::unique_ptr<IGfxQueue> create_gfx_queue_stub();
[[nodiscard]] std::unique_ptr<IGfxDevice> create_gfx_device_stub();
[[nodiscard]] std::unique_ptr<IGfxInstance> create_gfx_instance_stub();

}  // namespace vme::engine::gfx::contracts
