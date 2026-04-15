/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>

namespace vme::engine::render {

enum class BlendMode {
    kOpaque,
    kAlpha,
};

enum class RenderPassType {
    kBackground,
    kTiles,
    kOverlays,
};

struct MaterialDescriptor {
    std::string material_name{};
    BlendMode blend_mode{BlendMode::kOpaque};
    bool depth_test{true};
};

struct PassDescriptor {
    RenderPassType pass_type{RenderPassType::kBackground};
    MaterialDescriptor material{};
    std::uint32_t draw_calls{1};
};

[[nodiscard]] inline bool is_valid_material_descriptor(const MaterialDescriptor& material) {
    return !material.material_name.empty();
}

[[nodiscard]] inline bool is_valid_pass_descriptor(const PassDescriptor& pass) {
    return pass.draw_calls > 0 && is_valid_material_descriptor(pass.material);
}

}  // namespace vme::engine::render
