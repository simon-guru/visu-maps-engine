/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "engine/render/render_descriptors.hpp"

namespace vme::engine::render {

struct CacheInvalidationState {
    std::uint64_t style_revision{0};
    std::uint64_t pipeline_revision{0};
};

class SceneMaterialPipelineCache {
public:
    const MaterialDescriptor& resolve_material(
        const std::string& material_key,
        BlendMode blend_mode,
        bool depth_test);

    void invalidate_if_needed(const CacheInvalidationState& state);
    void invalidate_all();

    [[nodiscard]] std::uint64_t size() const noexcept { return materials_.size(); }
    [[nodiscard]] std::uint64_t cache_hits() const noexcept { return cache_hits_; }
    [[nodiscard]] std::uint64_t cache_misses() const noexcept { return cache_misses_; }

private:
    std::unordered_map<std::string, MaterialDescriptor> materials_{};
    CacheInvalidationState current_state_{};
    std::uint64_t cache_hits_{0};
    std::uint64_t cache_misses_{0};
};

}  // namespace vme::engine::render
