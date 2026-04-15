/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/render/scene_render_data.hpp"
#include "engine/tiles/render_snapshot.hpp"

namespace vme::engine::render {

[[nodiscard]] SceneRenderData build_scene_render_data_from_tiles_snapshot(
    const tiles::MapRenderSnapshot& snapshot);

}  // namespace vme::engine::render
