#include <cassert>

#include "engine/gfx/resources.hpp"

namespace {

using vme::engine::gfx::resources::BufferDesc;
using vme::engine::gfx::resources::BufferUsage;
using vme::engine::gfx::resources::ResourceErrorCode;
using vme::engine::gfx::resources::ResourceViewDesc;
using vme::engine::gfx::resources::ResourceViewType;
using vme::engine::gfx::resources::TextureDesc;
using vme::engine::gfx::resources::TextureDimension;
using vme::engine::gfx::resources::TextureFormat;
using vme::engine::gfx::resources::TextureUsage;
using vme::engine::gfx::resources::validate_buffer_desc;
using vme::engine::gfx::resources::validate_resource_view_desc;
using vme::engine::gfx::resources::validate_sampler_desc;
using vme::engine::gfx::resources::validate_shader_module_desc;
using vme::engine::gfx::resources::validate_texture_desc;

void test_buffer_validation_accepts_valid_descriptor() {
    // Garante baseline feliz para evitar falso-positivo em validações seguintes.
    const BufferDesc desc {
        .size_bytes = 64,
        .usage = BufferUsage::CopyDst | BufferUsage::Uniform,
    };

    assert(validate_buffer_desc(desc).ok());
}

void test_buffer_validation_rejects_unaligned_uniform_buffer() {
    // Uniform exige alinhamento mínimo para mapeamento consistente entre APIs.
    const BufferDesc desc {
        .size_bytes = 34,
        .usage = BufferUsage::Uniform,
    };

    const auto result = validate_buffer_desc(desc);
    assert(result.code == ResourceErrorCode::InvalidArgument);
}

void test_texture_validation_rejects_depth_storage_usage() {
    // Contrato proíbe depth+storage por baixa portabilidade entre backends.
    const TextureDesc desc {
        .dimension = TextureDimension::D2,
        .format = TextureFormat::D32Float,
        .usage = TextureUsage::Sampled | TextureUsage::Storage,
        .width = 256,
        .height = 256,
        .depth_or_layers = 1,
        .mip_levels = 1,
        .sample_count = 1,
    };

    const auto result = validate_texture_desc(desc);
    assert(result.code == ResourceErrorCode::IncompatibleUsage);
}

void test_texture_view_validation_checks_bounds_and_type() {
    // View não pode extrapolar mips/layers do recurso base.
    const TextureDesc texture_desc {
        .dimension = TextureDimension::D2,
        .format = TextureFormat::Rgba8Unorm,
        .usage = TextureUsage::Sampled,
        .width = 128,
        .height = 128,
        .depth_or_layers = 4,
        .mip_levels = 3,
        .sample_count = 1,
    };

    const ResourceViewDesc bad_view {
        .type = ResourceViewType::Texture2DArray,
        .base_mip_level = 2,
        .mip_level_count = 2,
        .base_array_layer = 0,
        .array_layer_count = 2,
    };

    const auto bad_result = validate_resource_view_desc(bad_view, texture_desc);
    assert(bad_result.code == ResourceErrorCode::OutOfRange);

    const ResourceViewDesc good_view {
        .type = ResourceViewType::Texture2DArray,
        .base_mip_level = 0,
        .mip_level_count = 1,
        .base_array_layer = 1,
        .array_layer_count = 2,
    };

    assert(validate_resource_view_desc(good_view, texture_desc).ok());
}

void test_buffer_view_validation_requires_matching_usage() {
    // View uniform só pode apontar para buffer com usage Uniform.
    const BufferDesc buffer_desc {
        .size_bytes = 256,
        .usage = BufferUsage::Storage | BufferUsage::CopyDst,
    };

    const ResourceViewDesc bad_view {
        .type = ResourceViewType::BufferUniform,
        .buffer_offset = 0,
        .buffer_size_bytes = 128,
    };

    const auto result = validate_resource_view_desc(bad_view, buffer_desc);
    assert(result.code == ResourceErrorCode::IncompatibleUsage);
}

void test_sampler_and_shader_validation_reject_invalid_descriptors() {
    // Descritores inválidos devem falhar cedo com erro de contrato.
    vme::engine::gfx::resources::SamplerDesc sampler_desc{};
    sampler_desc.anisotropy_enabled = true;
    sampler_desc.max_anisotropy = 1;

    const auto sampler_result = validate_sampler_desc(sampler_desc);
    assert(sampler_result.code == ResourceErrorCode::InvalidArgument);

    vme::engine::gfx::resources::ShaderModuleDesc shader_desc{};
    shader_desc.code.clear();

    const auto shader_result = validate_shader_module_desc(shader_desc);
    assert(shader_result.code == ResourceErrorCode::InvalidArgument);
}

}  // namespace

int main() {
    test_buffer_validation_accepts_valid_descriptor();
    test_buffer_validation_rejects_unaligned_uniform_buffer();
    test_texture_validation_rejects_depth_storage_usage();
    test_texture_view_validation_checks_bounds_and_type();
    test_buffer_view_validation_requires_matching_usage();
    test_sampler_and_shader_validation_reject_invalid_descriptors();
    return 0;
}
