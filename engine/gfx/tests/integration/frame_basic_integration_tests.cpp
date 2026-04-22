#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"
#include "engine/gfx/pipeline.hpp"
#include "engine/gfx/resources.hpp"

namespace {

using vme::engine::gfx::commands::CommandBufferDesc;
using vme::engine::gfx::commands::DrawCommand;
using vme::engine::gfx::contracts::QueueType;
using vme::engine::gfx::contracts::SubmitInfo;
using vme::engine::gfx::contracts::SwapchainDesc;
using vme::engine::gfx::contracts::SwapchainErrorCode;
using vme::engine::gfx::pipeline::ColorAttachmentDesc;
using vme::engine::gfx::pipeline::GraphicsPipelineDesc;
using vme::engine::gfx::pipeline::PipelineLayoutDesc;
using vme::engine::gfx::pipeline::ResourceSlotDesc;
using vme::engine::gfx::pipeline::ShaderStage;
using vme::engine::gfx::pipeline::ShaderStageDesc;
using vme::engine::gfx::resources::BufferDesc;
using vme::engine::gfx::resources::BufferUsage;
using vme::engine::gfx::resources::ResourceErrorCode;
using vme::engine::gfx::resources::ResourceViewDesc;
using vme::engine::gfx::resources::ResourceViewType;
using vme::engine::gfx::resources::SamplerDesc;
using vme::engine::gfx::resources::TextureDesc;
using vme::engine::gfx::resources::TextureDimension;
using vme::engine::gfx::resources::TextureFormat;
using vme::engine::gfx::resources::TextureUsage;

void test_instance_device_queue_swapchain_single_frame() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);
    assert(instance->adapter_count() > 0);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result = device_result.device->create_queue(QueueType::Graphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    const auto acquire = swapchain_result.swapchain->acquire_next_image();
    assert(acquire.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());
    const auto finish = encoder->finish(CommandBufferDesc {.label = "frame-0"});
    assert(finish.ok());

    SubmitInfo submit{};
    submit.command_buffer = finish.command_buffer.get();
    assert(queue_result.queue->submit(submit).ok());
    assert(queue_result.queue->process_next_submission().ok());

    const auto present = swapchain_result.swapchain->present(acquire.image_index);
    assert(present.ok());
}

void test_swapchain_rejects_invalid_recreate_and_wrong_present_order() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    const auto swapchain_result = device->create_swapchain(SwapchainDesc {.image_count = 2});
    assert(swapchain_result.ok());

    const auto invalid_recreate = swapchain_result.swapchain->recreate(SwapchainDesc {.image_count = 1});
    assert(invalid_recreate == SwapchainErrorCode::InvalidArgument);

    const auto wrong_present = swapchain_result.swapchain->present(0);
    assert(wrong_present.code == SwapchainErrorCode::InvalidState);
}

void test_device_pipeline_creation_uses_cache() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    GraphicsPipelineDesc desc{};
    desc.label = "basic-pipeline";
    desc.layout = PipelineLayoutDesc {
        .resource_slots = {ResourceSlotDesc {.set = 0, .binding = 0, .visibility = {ShaderStage::Vertex}}},
    };
    desc.shader_stages = {
        ShaderStageDesc {.stage = ShaderStage::Vertex, .entry_point = "main", .module_name = "vs_main"},
        ShaderStageDesc {.stage = ShaderStage::Fragment, .entry_point = "main", .module_name = "fs_main"},
    };
    desc.color_attachments = {ColorAttachmentDesc {}};

    const auto create_first = device->create_graphics_pipeline(desc);
    assert(create_first.ok());
    assert(!create_first.cache_hit);

    const auto create_second = device->create_graphics_pipeline(desc);
    assert(create_second.ok());
    assert(create_second.cache_hit);
    assert(create_first.pipeline_id == create_second.pipeline_id);
}

void test_device_creates_resources_and_views_with_contract_validation() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    const auto buffer_result = device->create_buffer(
        BufferDesc {.size_bytes = 256, .usage = BufferUsage::Storage | BufferUsage::CopyDst});
    assert(buffer_result.ok());

    const auto texture_result = device->create_texture(TextureDesc {.dimension = TextureDimension::D2,
                                                                    .format = TextureFormat::Rgba8Unorm,
                                                                    .usage = TextureUsage::Sampled,
                                                                    .width = 512,
                                                                    .height = 256,
                                                                    .depth_or_layers = 4,
                                                                    .mip_levels = 1,
                                                                    .sample_count = 1});
    assert(texture_result.ok());

    const auto sampler_result = device->create_sampler(SamplerDesc {});
    assert(sampler_result.ok());

    vme::engine::gfx::resources::ShaderModuleDesc shader_desc{};
    shader_desc.stage = vme::engine::gfx::resources::ShaderStage::Vertex;
    shader_desc.code = {0x03, 0x02, 0x23, 0x07};
    const auto shader_result = device->create_shader_module(shader_desc);
    assert(shader_result.ok());

    const auto buffer_view_result = device->create_resource_view(
        ResourceViewDesc {.type = ResourceViewType::BufferStorage, .buffer_offset = 0, .buffer_size_bytes = 128},
        *buffer_result.buffer);
    assert(buffer_view_result.ok());

    const auto texture_view_result = device->create_resource_view(
        ResourceViewDesc {.type = ResourceViewType::Texture2DArray,
                          .base_mip_level = 0,
                          .mip_level_count = 1,
                          .base_array_layer = 0,
                          .array_layer_count = 2},
        *texture_result.texture);
    assert(texture_view_result.ok());
}

void test_device_resource_caps_reject_unsupported_limits() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    const auto too_large_buffer = device->create_buffer(
        BufferDesc {.size_bytes = device->resource_caps().max_buffer_size_bytes + 1, .usage = BufferUsage::CopyDst});
    assert(too_large_buffer.error.code == ResourceErrorCode::CapabilityNotSupported);

    const auto unsupported_storage_texture =
        device->create_texture(TextureDesc {.dimension = TextureDimension::D2,
                                            .format = TextureFormat::Rgba16Float,
                                            .usage = TextureUsage::Storage,
                                            .width = 64,
                                            .height = 64,
                                            .depth_or_layers = 1,
                                            .mip_levels = 1,
                                            .sample_count = 1});
    assert(unsupported_storage_texture.error.code == ResourceErrorCode::CapabilityNotSupported);
}

}  // namespace

int main() {
    test_instance_device_queue_swapchain_single_frame();
    test_swapchain_rejects_invalid_recreate_and_wrong_present_order();
    test_device_pipeline_creation_uses_cache();
    test_device_creates_resources_and_views_with_contract_validation();
    test_device_resource_caps_reject_unsupported_limits();
    return 0;
}
