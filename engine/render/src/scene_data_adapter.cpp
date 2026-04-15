/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#include "engine/render/scene_data_adapter.hpp"

namespace vme::engine::render {

SceneRenderData build_scene_render_data_from_tiles_snapshot(const tiles::MapRenderSnapshot& snapshot) {
    SceneRenderData scene {};
    scene.render_background = snapshot.render_background;

    scene.tile_batches.reserve(snapshot.tile_batches.size());
    for (const auto& batch : snapshot.tile_batches) {
        scene.tile_batches.push_back(TileBatch {.tile_id = batch.tile_id, .draw_items = batch.drawable_count});
    }

    scene.overlay_batches.reserve(snapshot.overlay_batches.size());
    for (const auto& batch : snapshot.overlay_batches) {
        scene.overlay_batches.push_back(
            OverlayBatch {.overlay_id = batch.overlay_id, .draw_items = batch.drawable_count});
    }

    return scene;
}

}  // namespace vme::engine::render
