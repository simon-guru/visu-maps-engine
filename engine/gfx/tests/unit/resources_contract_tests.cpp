#include <cassert>

#include "engine/gfx/contracts/factory.hpp"
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
using vme::engine::gfx::resources::BackendApi;
using vme::engine::gfx::resources::translate_buffer_desc;
using vme::engine::gfx::resources::translate_texture_desc;

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

void test_caps_validation_rejects_backend_limits() {
    const BufferDesc buffer_desc {
        .size_bytes = 1024,
        .usage = BufferUsage::CopyDst,
    };

    vme::engine::gfx::resources::ResourceCaps caps{};
    caps.max_buffer_size_bytes = 512;
    const auto buffer_result = vme::engine::gfx::resources::validate_buffer_desc(buffer_desc, caps);
    assert(buffer_result.code == ResourceErrorCode::CapabilityNotSupported);

    const TextureDesc texture_desc {
        .dimension = TextureDimension::D2,
        .format = TextureFormat::Rgba16Float,
        .usage = TextureUsage::Storage,
        .width = 32,
        .height = 32,
        .depth_or_layers = 1,
        .mip_levels = 1,
        .sample_count = 1,
    };

    caps.max_buffer_size_bytes = 2048;
    caps.supports_storage_rgba16_float = false;
    const auto texture_result = vme::engine::gfx::resources::validate_texture_desc(texture_desc, caps);
    assert(texture_result.code == ResourceErrorCode::CapabilityNotSupported);
}

void test_backend_translation_respects_backend_specific_constraints() {
    vme::engine::gfx::resources::ResourceCaps caps{};
    caps.max_texture_dimension_2d = 1024;

    const auto buffer_result = translate_buffer_desc(
        BufferDesc {.size_bytes = 256, .usage = BufferUsage::CopySrc | BufferUsage::Vertex}, BackendApi::Vulkan, caps);
    assert(buffer_result.ok());
    assert(buffer_result.translated.backend_usage_mask != 0);

    const auto webgl_1d_result = translate_texture_desc(TextureDesc {.dimension = TextureDimension::D1,
                                                                     .format = TextureFormat::Rgba8Unorm,
                                                                     .usage = TextureUsage::Sampled,
                                                                     .width = 64,
                                                                     .height = 1,
                                                                     .depth_or_layers = 1,
                                                                     .mip_levels = 1,
                                                                     .sample_count = 1},
                                                        BackendApi::WebGL, caps);
    assert(webgl_1d_result.error.code == ResourceErrorCode::CapabilityNotSupported);
}

void test_lifecycle_tracking_counts_resource_create_destroy() {
    vme::engine::gfx::resources::debug::reset_lifecycle_stats();
    {
        auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
        const auto buffer = device->create_buffer(BufferDesc {.size_bytes = 64, .usage = BufferUsage::Storage});
        assert(buffer.ok());
        const auto texture = device->create_texture(TextureDesc {.dimension = TextureDimension::D2,
                                                                 .format = TextureFormat::Rgba8Unorm,
                                                                 .usage = TextureUsage::Sampled,
                                                                 .width = 32,
                                                                 .height = 32,
                                                                 .depth_or_layers = 1,
                                                                 .mip_levels = 1,
                                                                 .sample_count = 1});
        assert(texture.ok());
        const auto view = device->create_resource_view(
            ResourceViewDesc {.type = ResourceViewType::Texture2D, .mip_level_count = 1, .array_layer_count = 1},
            *texture.texture);
        assert(view.ok());
    }

    const auto stats = vme::engine::gfx::resources::debug::lifecycle_stats();
    assert(stats.buffers_created == 1 && stats.buffers_destroyed == 1);
    assert(stats.textures_created == 1 && stats.textures_destroyed == 1);
    assert(stats.views_created == 1 && stats.views_destroyed == 1);
}

}  // namespace

int main() {
    test_buffer_validation_accepts_valid_descriptor();
    test_buffer_validation_rejects_unaligned_uniform_buffer();
    test_texture_validation_rejects_depth_storage_usage();
    test_texture_view_validation_checks_bounds_and_type();
    test_buffer_view_validation_requires_matching_usage();
    test_sampler_and_shader_validation_reject_invalid_descriptors();
    test_caps_validation_rejects_backend_limits();
    test_backend_translation_respects_backend_specific_constraints();
    test_lifecycle_tracking_counts_resource_create_destroy();
    return 0;
}
