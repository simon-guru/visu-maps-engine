/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/resources/interfaces.hpp"

namespace vme::engine::gfx::resources::stub {

[[nodiscard]] std::unique_ptr<Buffer> create_buffer(BufferDesc desc);
[[nodiscard]] std::unique_ptr<Texture> create_texture(TextureDesc desc);
[[nodiscard]] std::unique_ptr<Sampler> create_sampler(SamplerDesc desc);
[[nodiscard]] std::unique_ptr<ShaderModule> create_shader_module(ShaderModuleDesc desc);
[[nodiscard]] std::unique_ptr<ResourceView> create_resource_view(ResourceViewDesc desc);

}  // namespace vme::engine::gfx::resources::stub
