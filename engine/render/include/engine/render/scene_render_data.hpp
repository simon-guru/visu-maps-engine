/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <vector>

namespace vme::engine::render {

struct TileBatch {
    std::uint32_t tile_id{0};
    std::uint32_t draw_items{1};
};

struct OverlayBatch {
    std::uint32_t overlay_id{0};
    std::uint32_t draw_items{1};
};

struct SceneRenderData {
    bool render_background{true};
    std::vector<TileBatch> tile_batches{};
    std::vector<OverlayBatch> overlay_batches{};
};

}  // namespace vme::engine::render
