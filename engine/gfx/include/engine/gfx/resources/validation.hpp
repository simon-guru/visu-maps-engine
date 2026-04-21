/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

#include "engine/gfx/resources/resource_desc.hpp"

namespace vme::engine::gfx::resources {

// Decisão de design:
// - conjunto restrito de sample counts mantém paridade com capacidades comuns de GPU/API.
[[nodiscard]] inline bool is_valid_sample_count(std::uint8_t sample_count) {
    return sample_count == 1 || sample_count == 2 || sample_count == 4 || sample_count == 8;
}

[[nodiscard]] inline bool is_depth_texture_format(TextureFormat format) {
    return format == TextureFormat::D24UnormS8 || format == TextureFormat::D32Float;
}

[[nodiscard]] inline bool texture_format_supports_storage(TextureFormat format) {
    return format != TextureFormat::D24UnormS8 && format != TextureFormat::D32Float &&
           format != TextureFormat::Undefined;
}

[[nodiscard]] inline bool texture_format_supported_for_storage_by_caps(const TextureFormat format,
                                                                       const ResourceCaps& caps) {
    if (format == TextureFormat::R32Float) {
        return caps.supports_storage_r32_float;
    }
    if (format == TextureFormat::Rgba16Float) {
        return caps.supports_storage_rgba16_float;
    }

    return texture_format_supports_storage(format);
}

[[nodiscard]] inline ResourceError validate_buffer_desc(const BufferDesc& desc) {
    // Motivo: buffer de tamanho zero cria objeto inválido em praticamente todos backends.
    if (desc.size_bytes == 0) {
        return {ResourceErrorCode::InvalidArgument, "buffer size_bytes must be greater than zero"};
    }

    // Motivo: usage explícito evita ambiguidade de permissões de acesso.
    if (desc.usage == BufferUsage::None) {
        return {ResourceErrorCode::InvalidArgument, "buffer usage cannot be None"};
    }

    // Motivo: alinhamento mínimo de uniform/storage evita UB e falhas de bind.
    if (has_flag(desc.usage, BufferUsage::Uniform) && (desc.size_bytes % 16u) != 0u) {
        return {ResourceErrorCode::InvalidArgument,
                "uniform buffer size_bytes must be aligned to 16 bytes"};
    }

    if (has_flag(desc.usage, BufferUsage::Storage) && (desc.size_bytes % 16u) != 0u) {
        return {ResourceErrorCode::InvalidArgument,
                "storage buffer size_bytes must be aligned to 16 bytes"};
    }

    if (has_flag(desc.usage, BufferUsage::Vertex) && (desc.size_bytes % 4u) != 0u) {
        return {ResourceErrorCode::InvalidArgument,
                "vertex buffer size_bytes must be aligned to 4 bytes"};
    }

    if (has_flag(desc.usage, BufferUsage::Index) && (desc.size_bytes % 2u) != 0u) {
        return {ResourceErrorCode::InvalidArgument,
                "index buffer size_bytes must be aligned to 2 bytes"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_buffer_desc(const BufferDesc& desc, const ResourceCaps& caps) {
    // Estratégia em duas etapas:
    // 1) valida regras universais de contrato;
    // 2) valida limites específicos do backend/dispositivo.
    const auto base_error = validate_buffer_desc(desc);
    if (!base_error.ok()) {
        return base_error;
    }

    if (desc.size_bytes > caps.max_buffer_size_bytes) {
        return {ResourceErrorCode::CapabilityNotSupported,
                "buffer size_bytes exceeds backend max_buffer_size_bytes"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_texture_desc(const TextureDesc& desc) {
    // Motivo: formato indefinido inviabiliza criação consistente em qualquer backend.
    if (desc.format == TextureFormat::Undefined) {
        return {ResourceErrorCode::InvalidArgument, "texture format cannot be Undefined"};
    }

    if (desc.usage == TextureUsage::None) {
        return {ResourceErrorCode::InvalidArgument, "texture usage cannot be None"};
    }

    if (desc.width == 0 || desc.height == 0 || desc.depth_or_layers == 0) {
        return {ResourceErrorCode::InvalidArgument,
                "texture width, height and depth_or_layers must be greater than zero"};
    }

    if (desc.mip_levels == 0) {
        return {ResourceErrorCode::InvalidArgument, "texture mip_levels must be greater than zero"};
    }

    if (!is_valid_sample_count(desc.sample_count)) {
        return {ResourceErrorCode::InvalidArgument,
                "texture sample_count must be one of {1,2,4,8}"};
    }

    if (desc.dimension == TextureDimension::D1 && (desc.height != 1 || desc.depth_or_layers != 1)) {
        return {ResourceErrorCode::InvalidArgument,
                "1D texture must have height=1 and depth_or_layers=1"};
    }

    // Motivo: combinações MSAA+3D/mips variam entre APIs; aqui fixamos contrato conservador.
    if (desc.dimension == TextureDimension::D3 && desc.sample_count != 1) {
        return {ResourceErrorCode::InvalidArgument,
                "3D texture does not support multisampling in this abstraction"};
    }

    if (desc.sample_count > 1 && desc.dimension != TextureDimension::D2) {
        return {ResourceErrorCode::InvalidArgument,
                "multisampled textures must be 2D in this abstraction"};
    }

    if (desc.sample_count > 1 && desc.mip_levels != 1) {
        return {ResourceErrorCode::InvalidArgument,
                "multisampled textures cannot declare mip_levels greater than one"};
    }

    // Motivo: formatos de profundidade não são storage texturing portável no contrato base.
    if (is_depth_texture_format(desc.format) && has_flag(desc.usage, TextureUsage::Storage)) {
        return {ResourceErrorCode::IncompatibleUsage,
                "depth texture format cannot be used with Storage usage"};
    }

    if (has_flag(desc.usage, TextureUsage::Storage) && !texture_format_supports_storage(desc.format)) {
        return {ResourceErrorCode::IncompatibleUsage,
                "texture format is not compatible with Storage usage"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_texture_desc(const TextureDesc& desc, const ResourceCaps& caps) {
    // Estratégia equivalente à de buffer para manter previsibilidade da API pública.
    const auto base_error = validate_texture_desc(desc);
    if (!base_error.ok()) {
        return base_error;
    }

    if (desc.mip_levels > caps.max_texture_mip_levels) {
        return {ResourceErrorCode::CapabilityNotSupported,
                "texture mip_levels exceeds backend max_texture_mip_levels"};
    }

    if (desc.dimension == TextureDimension::D1) {
        // 1D usa apenas largura; demais eixos já foram fixados pela validação base.
        if (desc.width > caps.max_texture_dimension_1d) {
            return {ResourceErrorCode::CapabilityNotSupported,
                    "1D texture width exceeds backend max_texture_dimension_1d"};
        }
    } else if (desc.dimension == TextureDimension::D2) {
        // 2D trata depth_or_layers como array layers.
        if (desc.width > caps.max_texture_dimension_2d || desc.height > caps.max_texture_dimension_2d) {
            return {ResourceErrorCode::CapabilityNotSupported,
                    "2D texture dimensions exceed backend max_texture_dimension_2d"};
        }
        if (desc.depth_or_layers > caps.max_texture_array_layers) {
            return {ResourceErrorCode::CapabilityNotSupported,
                    "2D texture array layers exceed backend max_texture_array_layers"};
        }
    } else {
        // 3D usa depth_or_layers como profundidade volumétrica.
        if (desc.width > caps.max_texture_dimension_3d || desc.height > caps.max_texture_dimension_3d ||
            desc.depth_or_layers > caps.max_texture_dimension_3d) {
            return {ResourceErrorCode::CapabilityNotSupported,
                    "3D texture dimensions exceed backend max_texture_dimension_3d"};
        }
    }

    if (has_flag(desc.usage, TextureUsage::Storage) &&
        !texture_format_supported_for_storage_by_caps(desc.format, caps)) {
        return {ResourceErrorCode::CapabilityNotSupported,
                "texture format is not supported for Storage usage by this backend"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_sampler_desc(const SamplerDesc& desc) {
    // Motivo: range de LOD invertido é estado inválido e difícil de depurar no runtime.
    if (desc.min_lod > desc.max_lod) {
        return {ResourceErrorCode::InvalidArgument, "sampler min_lod cannot be greater than max_lod"};
    }

    if (desc.anisotropy_enabled && desc.max_anisotropy < 2) {
        return {ResourceErrorCode::InvalidArgument,
                "anisotropic sampling requires max_anisotropy >= 2"};
    }

    if (!desc.anisotropy_enabled && desc.max_anisotropy != 1) {
        return {ResourceErrorCode::InvalidArgument,
                "max_anisotropy must remain 1 when anisotropy is disabled"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_sampler_desc(const SamplerDesc& desc, const ResourceCaps& caps) {
    // Mantém semântica: contrato primeiro, capacidade depois.
    const auto base_error = validate_sampler_desc(desc);
    if (!base_error.ok()) {
        return base_error;
    }

    if (desc.anisotropy_enabled && desc.max_anisotropy > caps.max_sampler_anisotropy) {
        return {ResourceErrorCode::CapabilityNotSupported,
                "sampler max_anisotropy exceeds backend max_sampler_anisotropy"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_shader_module_desc(const ShaderModuleDesc& desc) {
    // Motivo: sem código/entry point, não há como materializar pipeline deterministicamente.
    if (desc.code.empty()) {
        return {ResourceErrorCode::InvalidArgument, "shader module code cannot be empty"};
    }

    if (desc.entry_point.empty()) {
        return {ResourceErrorCode::InvalidArgument, "shader module entry_point cannot be empty"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_resource_view_desc(const ResourceViewDesc& desc,
                                                               const BufferDesc& buffer_desc) {
    // Motivo: view de buffer só é válida para classes de binding de buffer.
    if (desc.type != ResourceViewType::BufferUniform && desc.type != ResourceViewType::BufferStorage) {
        return {ResourceErrorCode::InvalidArgument,
                "buffer resource view type must be BufferUniform or BufferStorage"};
    }

    if (desc.buffer_size_bytes == 0) {
        return {ResourceErrorCode::InvalidArgument,
                "buffer resource view buffer_size_bytes must be greater than zero"};
    }

    // Motivo: garante coerência entre subrange da view e o recurso base.
    if (desc.buffer_offset + desc.buffer_size_bytes > buffer_desc.size_bytes) {
        return {ResourceErrorCode::OutOfRange,
                "buffer resource view range exceeds buffer size"};
    }

    if (desc.type == ResourceViewType::BufferUniform && !has_flag(buffer_desc.usage, BufferUsage::Uniform)) {
        return {ResourceErrorCode::IncompatibleUsage,
                "buffer resource view type BufferUniform requires Uniform usage"};
    }

    if (desc.type == ResourceViewType::BufferStorage && !has_flag(buffer_desc.usage, BufferUsage::Storage)) {
        return {ResourceErrorCode::IncompatibleUsage,
                "buffer resource view type BufferStorage requires Storage usage"};
    }

    if ((desc.buffer_offset % 16u) != 0u) {
        return {ResourceErrorCode::InvalidArgument, "buffer resource view offset must be aligned to 16 bytes"};
    }

    return ResourceError::ok_result();
}

[[nodiscard]] inline ResourceError validate_resource_view_desc(const ResourceViewDesc& desc,
                                                               const TextureDesc& texture_desc) {
    // Motivo: impede cruzamento indevido entre taxonomias buffer/texture.
    if (desc.type == ResourceViewType::BufferUniform || desc.type == ResourceViewType::BufferStorage) {
        return {ResourceErrorCode::InvalidArgument,
                "texture resource view cannot use a buffer view type"};
    }

    if (desc.base_mip_level >= texture_desc.mip_levels || desc.mip_level_count == 0) {
        return {ResourceErrorCode::OutOfRange,
                "texture resource view mip range is out of bounds"};
    }

    if (desc.base_mip_level + desc.mip_level_count > texture_desc.mip_levels) {
        return {ResourceErrorCode::OutOfRange,
                "texture resource view mip range exceeds texture mip levels"};
    }

    if (desc.base_array_layer >= texture_desc.depth_or_layers || desc.array_layer_count == 0) {
        return {ResourceErrorCode::OutOfRange,
                "texture resource view array range is out of bounds"};
    }

    if (desc.base_array_layer + desc.array_layer_count > texture_desc.depth_or_layers) {
        return {ResourceErrorCode::OutOfRange,
                "texture resource view array range exceeds texture depth_or_layers"};
    }

    // Motivo: contrato público não permite reinterpretar formato por padrão.
    if (desc.format != TextureFormat::Undefined && desc.format != texture_desc.format) {
        return {ResourceErrorCode::IncompatibleUsage,
                "texture resource view format must match the base texture format"};
    }

    if (!has_flag(texture_desc.usage, TextureUsage::Sampled) &&
        !has_flag(texture_desc.usage, TextureUsage::Storage)) {
        return {ResourceErrorCode::IncompatibleUsage,
                "texture resource view requires Sampled or Storage usage in base texture"};
    }

    return ResourceError::ok_result();
}

}  // namespace vme::engine::gfx::resources
