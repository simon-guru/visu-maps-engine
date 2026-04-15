/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include "engine/render/material_pipeline_cache.hpp"
#include "engine/services/theme_style_events.hpp"

namespace vme::engine::render {

inline void apply_theme_style_event_to_material_cache(
    SceneMaterialPipelineCache& cache,
    const services::ThemeStyleChangedEvent& event) {
    cache.invalidate_if_needed(CacheInvalidationState {
        .style_revision = event.style_revision,
        .pipeline_revision = event.pipeline_revision,
    });
}

}  // namespace vme::engine::render
