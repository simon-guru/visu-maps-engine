/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <vector>

#include "engine/render/render_descriptors.hpp"
#include "engine/render/scene_render_data.hpp"

namespace vme::engine::render {

struct FramePlan {
    std::vector<PassDescriptor> passes{};
};

[[nodiscard]] FramePlan build_default_frame_plan();
[[nodiscard]] FramePlan build_frame_plan_from_scene(const SceneRenderData& scene_data);

}  // namespace vme::engine::render
