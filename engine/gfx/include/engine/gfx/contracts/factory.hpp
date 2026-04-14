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

namespace vme::engine::gfx::contracts {

[[nodiscard]] std::unique_ptr<IGfxQueue> create_gfx_queue_stub();
[[nodiscard]] std::unique_ptr<IGfxDevice> create_gfx_device_stub();
[[nodiscard]] std::unique_ptr<IGfxInstance> create_gfx_instance_stub();

}  // namespace vme::engine::gfx::contracts
