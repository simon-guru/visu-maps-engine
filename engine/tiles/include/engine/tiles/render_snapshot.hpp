/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <vector>

namespace vme::engine::tiles {

struct TileRenderBatch {
    std::uint32_t tile_id{0};
    std::uint32_t drawable_count{1};
};

struct OverlayRenderBatch {
    std::uint32_t overlay_id{0};
    std::uint32_t drawable_count{1};
};

struct MapRenderSnapshot {
    bool render_background{true};
    std::vector<TileRenderBatch> tile_batches{};
    std::vector<OverlayRenderBatch> overlay_batches{};
    std::uint64_t style_revision{0};
    std::uint64_t pipeline_revision{0};
};

}  // namespace vme::engine::tiles
