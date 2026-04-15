/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#include "engine/tiles/render_snapshot_cache.hpp"

namespace vme::engine::tiles {

void RenderSnapshotCache::put_snapshot(const MapRenderSnapshot& snapshot) {
    latest_snapshot_ = snapshot;
}

bool RenderSnapshotCache::has_snapshot() const noexcept {
    return latest_snapshot_.has_value();
}

std::optional<MapRenderSnapshot> RenderSnapshotCache::latest_snapshot() const {
    return latest_snapshot_;
}

}  // namespace vme::engine::tiles
