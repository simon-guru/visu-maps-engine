/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/render/material_pipeline_cache.hpp"

namespace vme::engine::render {

const MaterialDescriptor& SceneMaterialPipelineCache::resolve_material(
    const std::string& material_key,
    BlendMode blend_mode,
    bool depth_test) {
    auto it = materials_.find(material_key);
    if (it != materials_.end()) {
        ++cache_hits_;
        return it->second;
    }

    ++cache_misses_;
    auto [inserted_it, _] = materials_.emplace(
        material_key,
        MaterialDescriptor {
            .material_name = material_key,
            .blend_mode = blend_mode,
            .depth_test = depth_test,
        });

    return inserted_it->second;
}

void SceneMaterialPipelineCache::invalidate_if_needed(const CacheInvalidationState& state) {
    if (state.style_revision != current_state_.style_revision ||
        state.pipeline_revision != current_state_.pipeline_revision) {
        invalidate_all();
        current_state_ = state;
    }
}

void SceneMaterialPipelineCache::invalidate_all() {
    materials_.clear();
}

}  // namespace vme::engine::render
