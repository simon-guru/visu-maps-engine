/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/gfx/commands/i_command_encoder.hpp"
#include "engine/gfx/contracts/i_gfx_instance.hpp"

namespace vme::engine::render {

struct BasicFrameResult {
    bool ok{false};
    const char* message{nullptr};
};

/**
 * @brief Caminho mínimo de quadro: acquire -> encode -> submit -> present.
 */
BasicFrameResult run_basic_frame(
    gfx::contracts::IGfxInstance& instance,
    gfx::contracts::IGfxDevice& device,
    gfx::contracts::IGfxQueue& queue,
    gfx::contracts::IGfxSwapchain& swapchain,
    gfx::commands::ICommandEncoder& encoder);

}  // namespace vme::engine::render
