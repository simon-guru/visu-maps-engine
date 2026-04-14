/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/render/basic_frame_renderer.hpp"

namespace vme::engine::render {

BasicFrameResult run_basic_frame(
    gfx::contracts::IGfxInstance& instance,
    gfx::contracts::IGfxDevice& /*device*/,
    gfx::contracts::IGfxQueue& queue,
    gfx::contracts::IGfxSwapchain& swapchain,
    gfx::commands::ICommandEncoder& encoder) {
    if (instance.adapter_count() == 0) {
        return {false, "no graphics adapters available"};
    }

    const auto acquire = swapchain.acquire_next_image();
    if (!acquire.ok()) {
        return {false, "swapchain acquire failed"};
    }

    const auto draw_error = encoder.draw(gfx::commands::DrawCommand {.vertex_count = 3});
    if (!draw_error.ok()) {
        return {false, "command encoding failed"};
    }

    const auto finish_result =
        encoder.finish(gfx::commands::CommandBufferDesc {.label = "render.basic_frame"});
    if (!finish_result.ok()) {
        return {false, "command buffer finish failed"};
    }

    gfx::contracts::SubmitInfo submit_info {};
    submit_info.command_buffer = finish_result.command_buffer.get();

    const auto submit_result = queue.submit(submit_info);
    if (!submit_result.ok()) {
        return {false, "queue submit failed"};
    }

    const auto process_result = queue.process_next_submission();
    if (!process_result.ok()) {
        return {false, "queue process failed"};
    }

    const auto present = swapchain.present(acquire.image_index);
    if (!present.ok()) {
        return {false, "swapchain present failed"};
    }

    return {true, nullptr};
}

}  // namespace vme::engine::render
