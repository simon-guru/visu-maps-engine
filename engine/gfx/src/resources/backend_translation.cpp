/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/resources/backend_translation.hpp"

#include "engine/gfx/resources/validation.hpp"

namespace vme::engine::gfx::resources {
namespace {

// Decisão:
// - o tradutor usa códigos abstratos para desacoplar contrato público de headers nativos;
// - integração com APIs reais (Vk/MTL/GL) ficará em camada posterior sem quebrar assinatura.
std::uint32_t translate_buffer_usage(BufferUsage usage, BackendApi api) {
    // Nota: valores são códigos abstratos do tradutor (não enums nativos reais).
    const auto base = api == BackendApi::Vulkan ? 0x1000u : api == BackendApi::Metal ? 0x2000u : 0x3000u;
    std::uint32_t mask = 0;

    if (has_flag(usage, BufferUsage::CopySrc)) mask |= (base | 0x01u);
    if (has_flag(usage, BufferUsage::CopyDst)) mask |= (base | 0x02u);
    if (has_flag(usage, BufferUsage::Vertex)) mask |= (base | 0x04u);
    if (has_flag(usage, BufferUsage::Index)) mask |= (base | 0x08u);
    if (has_flag(usage, BufferUsage::Uniform)) mask |= (base | 0x10u);
    if (has_flag(usage, BufferUsage::Storage)) mask |= (base | 0x20u);

    return mask;
}

std::uint32_t texture_target_code(TextureDimension dimension, BackendApi api) {
    if (api == BackendApi::WebGL) {
        return dimension == TextureDimension::D3 ? 0x806Fu : 0x0DE1u;  // 2D/3D
    }

    if (api == BackendApi::Metal) {
        return dimension == TextureDimension::D1 ? 1u : dimension == TextureDimension::D2 ? 2u : 3u;
    }

    return dimension == TextureDimension::D1 ? 10u : dimension == TextureDimension::D2 ? 20u : 30u;
}

std::uint32_t format_code(TextureFormat format, BackendApi api) {
    return static_cast<std::uint32_t>(format) + (api == BackendApi::Vulkan ? 100u : api == BackendApi::Metal ? 200u : 300u);
}

}  // namespace

TranslateBufferResult translate_buffer_desc(const BufferDesc& desc, BackendApi api,
                                            const ResourceCaps& caps) {
    // Primeiro validamos descriptor + caps, depois traduzimos.
    const auto validation = validate_buffer_desc(desc, caps);
    if (!validation.ok()) {
        return {{}, validation};
    }

    return {
        TranslatedBufferDesc {
            .label = desc.label,
            .size_bytes = desc.size_bytes,
            .backend_usage_mask = translate_buffer_usage(desc.usage, api),
        },
        ResourceError::ok_result(),
    };
}

TranslateTextureResult translate_texture_desc(const TextureDesc& desc, BackendApi api,
                                              const ResourceCaps& caps) {
    // Mesmo pipeline de segurança para textura.
    const auto validation = validate_texture_desc(desc, caps);
    if (!validation.ok()) {
        return {{}, validation};
    }

    if (api == BackendApi::WebGL && desc.dimension == TextureDimension::D1) {
        // Decisão:
        // - WebGL moderno não expõe alvo 1D dedicado; preferimos falhar explicitamente
        //   em vez de conversão implícita para 2D.
        return {{}, {ResourceErrorCode::CapabilityNotSupported,
                     "WebGL translation does not expose dedicated 1D texture target"}};
    }

    return {
        TranslatedTextureDesc {
            .label = desc.label,
            .width = desc.width,
            .height = desc.height,
            .depth_or_layers = desc.depth_or_layers,
            .mip_levels = desc.mip_levels,
            .sample_count = desc.sample_count,
            .backend_usage_mask = static_cast<std::uint32_t>(desc.usage),
            .backend_format_code = format_code(desc.format, api),
            .backend_target_code = texture_target_code(desc.dimension, api),
        },
        ResourceError::ok_result(),
    };
}

}  // namespace vme::engine::gfx::resources
