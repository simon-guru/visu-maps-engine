/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/contracts/i_gfx_device.hpp"
#include "engine/gfx/contracts/i_gfx_queue.hpp"

namespace vme::engine::gfx::contracts {

/**
 * @brief Cria implementação stub de `IGfxQueue` com fila interna e sinalização básica.
 */
[[nodiscard]] std::unique_ptr<IGfxQueue> create_gfx_queue_stub();

/**
 * @brief Cria implementação stub de `IGfxDevice` para validação end-to-end.
 */
[[nodiscard]] std::unique_ptr<IGfxDevice> create_gfx_device_stub();

}  // namespace vme::engine::gfx::contracts
