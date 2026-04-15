/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/render/frame_plan.hpp"

namespace vme::engine::render {

FramePlan build_default_frame_plan() {
    FramePlan plan {};

    plan.passes.push_back(PassDescriptor {
        .pass_type = RenderPassType::kBackground,
        .material = MaterialDescriptor {.material_name = "background.base", .blend_mode = BlendMode::kOpaque, .depth_test = false},
        .draw_calls = 1,
    });

    plan.passes.push_back(PassDescriptor {
        .pass_type = RenderPassType::kTiles,
        .material = MaterialDescriptor {.material_name = "tiles.standard", .blend_mode = BlendMode::kOpaque, .depth_test = true},
        .draw_calls = 2,
    });

    plan.passes.push_back(PassDescriptor {
        .pass_type = RenderPassType::kOverlays,
        .material = MaterialDescriptor {.material_name = "overlays.alpha", .blend_mode = BlendMode::kAlpha, .depth_test = false},
        .draw_calls = 1,
    });

    return plan;
}

FramePlan build_frame_plan_from_scene(const SceneRenderData& scene_data) {
    FramePlan plan {};

    if (scene_data.render_background) {
        plan.passes.push_back(PassDescriptor {
            .pass_type = RenderPassType::kBackground,
            .material = MaterialDescriptor {.material_name = "background.base", .blend_mode = BlendMode::kOpaque, .depth_test = false},
            .draw_calls = 1,
        });
    }

    std::uint32_t tile_draw_count = 0;
    for (const auto& batch : scene_data.tile_batches) {
        tile_draw_count += batch.draw_items;
    }

    if (tile_draw_count > 0) {
        plan.passes.push_back(PassDescriptor {
            .pass_type = RenderPassType::kTiles,
            .material = MaterialDescriptor {.material_name = "tiles.standard", .blend_mode = BlendMode::kOpaque, .depth_test = true},
            .draw_calls = tile_draw_count,
        });
    }

    std::uint32_t overlay_draw_count = 0;
    for (const auto& batch : scene_data.overlay_batches) {
        overlay_draw_count += batch.draw_items;
    }

    if (overlay_draw_count > 0) {
        plan.passes.push_back(PassDescriptor {
            .pass_type = RenderPassType::kOverlays,
            .material = MaterialDescriptor {.material_name = "overlays.alpha", .blend_mode = BlendMode::kAlpha, .depth_test = false},
            .draw_calls = overlay_draw_count,
        });
    }

    return plan;
}

}  // namespace vme::engine::render
