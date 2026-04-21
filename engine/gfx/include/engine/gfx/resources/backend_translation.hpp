/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>

#include "engine/gfx/resources/resource_desc.hpp"

namespace vme::engine::gfx::resources {

enum class BackendApi : std::uint8_t {
    Vulkan,
    Metal,
    WebGL,
};

struct TranslatedBufferDesc {
    std::string label{};
    std::uint64_t size_bytes{0};
    std::uint32_t backend_usage_mask{0};
};

struct TranslatedTextureDesc {
    std::string label{};
    std::uint32_t width{0};
    std::uint32_t height{0};
    std::uint32_t depth_or_layers{0};
    std::uint32_t mip_levels{0};
    std::uint8_t sample_count{1};
    std::uint32_t backend_usage_mask{0};
    std::uint32_t backend_format_code{0};
    std::uint32_t backend_target_code{0};
};

struct TranslateBufferResult {
    TranslatedBufferDesc translated{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok(); }
};

struct TranslateTextureResult {
    TranslatedTextureDesc translated{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok(); }
};

[[nodiscard]] TranslateBufferResult translate_buffer_desc(const BufferDesc& desc, BackendApi api,
                                                          const ResourceCaps& caps);
[[nodiscard]] TranslateTextureResult translate_texture_desc(const TextureDesc& desc, BackendApi api,
                                                            const ResourceCaps& caps);

}  // namespace vme::engine::gfx::resources
