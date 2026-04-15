/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <optional>

#include "engine/tiles/render_snapshot.hpp"

namespace vme::engine::tiles {

class RenderSnapshotCache {
public:
    void put_snapshot(const MapRenderSnapshot& snapshot);
    [[nodiscard]] bool has_snapshot() const noexcept;
    [[nodiscard]] std::optional<MapRenderSnapshot> latest_snapshot() const;

private:
    std::optional<MapRenderSnapshot> latest_snapshot_{};
};

}  // namespace vme::engine::tiles
