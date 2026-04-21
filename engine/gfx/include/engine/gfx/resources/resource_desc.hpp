/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "engine/gfx/resources/resource_common.hpp"

namespace vme::engine::gfx::resources {

/**
 * @brief Erro textual simples para validações de contrato.
 *
 * Decisão de design:
 * - struct pequena e sem alocação indireta obrigatória;
 * - compatível com validação inline em headers (sem custo de linkage extra).
 */
struct ResourceError {
    ResourceErrorCode code{ResourceErrorCode::Ok};
    std::string message{};

    [[nodiscard]] bool ok() const noexcept { return code == ResourceErrorCode::Ok; }

    static ResourceError ok_result() noexcept { return {}; }
};

enum class ShaderStage : std::uint8_t {
    Vertex,
    Fragment,
    Compute,
};

enum class MemoryHint : std::uint8_t {
    DeviceLocal,
    HostVisible,
    HostCoherent,
    HostCached,
};

enum class BufferUsage : std::uint32_t {
    None = 0,
    CopySrc = 1u << 0u,
    CopyDst = 1u << 1u,
    Vertex = 1u << 2u,
    Index = 1u << 3u,
    Uniform = 1u << 4u,
    Storage = 1u << 5u,
    Indirect = 1u << 6u,
    MapRead = 1u << 7u,
    MapWrite = 1u << 8u,
};

template <>
struct EnableBitMaskOps<BufferUsage> : std::true_type {};

/**
 * @brief Descritor explícito de buffer.
 *
 * Decisão de design:
 * - tamanho/uso/memory_hint são obrigatórios no contrato para evitar defaults implícitos
 *   divergentes entre Vulkan/Metal/GLES/WebGL.
 */
struct BufferDesc {
    std::string label{};
    std::uint64_t size_bytes{0};
    BufferUsage usage{BufferUsage::None};
    MemoryHint memory_hint{MemoryHint::DeviceLocal};
};

enum class TextureDimension : std::uint8_t {
    D1,
    D2,
    D3,
};

enum class TextureFormat : std::uint8_t {
    Undefined,
    R8Unorm,
    Rg8Unorm,
    Rgba8Unorm,
    Bgra8Unorm,
    Rgba16Float,
    R32Float,
    D24UnormS8,
    D32Float,
};

enum class TextureUsage : std::uint32_t {
    None = 0,
    CopySrc = 1u << 0u,
    CopyDst = 1u << 1u,
    Sampled = 1u << 2u,
    Storage = 1u << 3u,
    RenderTarget = 1u << 4u,
};

template <>
struct EnableBitMaskOps<TextureUsage> : std::true_type {};

struct TextureDesc {
    std::string label{};
    TextureDimension dimension{TextureDimension::D2};
    TextureFormat format{TextureFormat::Rgba8Unorm};
    TextureUsage usage{TextureUsage::Sampled};
    MemoryHint memory_hint{MemoryHint::DeviceLocal};
    std::uint32_t width{1};
    std::uint32_t height{1};
    std::uint32_t depth_or_layers{1};
    std::uint32_t mip_levels{1};
    std::uint8_t sample_count{1};
};

enum class FilterMode : std::uint8_t {
    Nearest,
    Linear,
};

enum class MipmapMode : std::uint8_t {
    Nearest,
    Linear,
};

enum class AddressMode : std::uint8_t {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class CompareOp : std::uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

struct SamplerDesc {
    std::string label{};
    FilterMode mag_filter{FilterMode::Linear};
    FilterMode min_filter{FilterMode::Linear};
    MipmapMode mipmap_mode{MipmapMode::Linear};
    AddressMode address_u{AddressMode::Repeat};
    AddressMode address_v{AddressMode::Repeat};
    AddressMode address_w{AddressMode::Repeat};
    float mip_lod_bias{0.0f};
    float min_lod{0.0f};
    float max_lod{32.0f};
    bool anisotropy_enabled{false};
    std::uint8_t max_anisotropy{1};
    bool compare_enabled{false};
    CompareOp compare_op{CompareOp::Always};
};

enum class ShaderLanguage : std::uint8_t {
    SpirV,
    Msl,
    Glsl,
    Wgsl,
};

struct ShaderModuleDesc {
    std::string label{};
    ShaderStage stage{ShaderStage::Vertex};
    ShaderLanguage language{ShaderLanguage::SpirV};
    std::vector<std::uint8_t> code{};
    std::string entry_point{"main"};
};

enum class ResourceViewType : std::uint8_t {
    BufferUniform,
    BufferStorage,
    Texture1D,
    Texture2D,
    Texture2DArray,
    Texture3D,
};

/**
 * @brief Visão tipada sem expor handle de backend.
 *
 * Decisão de design:
 * - permite bind seguro (uniform/storage/texture view) com verificação de coerência
 *   frente ao recurso base, mantendo encapsulamento do backend.
 */
struct ResourceViewDesc {
    std::string label{};
    ResourceViewType type{ResourceViewType::Texture2D};
    TextureFormat format{TextureFormat::Undefined};

    std::uint64_t buffer_offset{0};
    std::uint64_t buffer_size_bytes{0};

    std::uint32_t base_mip_level{0};
    std::uint32_t mip_level_count{1};
    std::uint32_t base_array_layer{0};
    std::uint32_t array_layer_count{1};
};

}  // namespace vme::engine::gfx::resources
