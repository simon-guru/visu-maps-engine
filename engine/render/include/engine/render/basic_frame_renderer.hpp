/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

#include "engine/gfx/commands/i_command_encoder.hpp"
#include "engine/gfx/contracts/i_gfx_instance.hpp"
#include "engine/render/frame_plan.hpp"

namespace vme::engine::render {

struct BasicFrameResult {
    bool ok{false};
    const char* message{nullptr};
    std::uint32_t encoded_pass_count{0};
    std::uint32_t encoded_draw_count{0};
};

BasicFrameResult run_basic_frame(
    gfx::contracts::IGfxInstance& instance,
    gfx::contracts::IGfxDevice& device,
    gfx::contracts::IGfxQueue& queue,
    gfx::contracts::IGfxSwapchain& swapchain,
    gfx::commands::ICommandEncoder& encoder,
    const FramePlan& frame_plan);

[[nodiscard]] BasicFrameResult run_basic_frame_default(
    gfx::contracts::IGfxInstance& instance,
    gfx::contracts::IGfxDevice& device,
    gfx::contracts::IGfxQueue& queue,
    gfx::contracts::IGfxSwapchain& swapchain,
    gfx::commands::ICommandEncoder& encoder);

[[nodiscard]] BasicFrameResult run_basic_frame_scene(
    gfx::contracts::IGfxInstance& instance,
    gfx::contracts::IGfxDevice& device,
    gfx::contracts::IGfxQueue& queue,
    gfx::contracts::IGfxSwapchain& swapchain,
    gfx::commands::ICommandEncoder& encoder,
    const SceneRenderData& scene_data);

}  // namespace vme::engine::render
