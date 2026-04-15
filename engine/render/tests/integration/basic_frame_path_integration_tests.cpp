#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"
#include "engine/render/basic_frame_renderer.hpp"
#include "engine/render/material_pipeline_cache.hpp"
#include "engine/render/scene_data_adapter.hpp"

namespace {

void test_runs_basic_frame_path_with_gfx_stubs() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::kGraphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(vme::engine::gfx::contracts::SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder);

    const auto result = vme::engine::render::run_basic_frame_default(
        *instance, *device_result.device, *queue_result.queue, *swapchain_result.swapchain, *encoder);

    assert(result.ok);
    assert(result.encoded_pass_count == 3);
    assert(result.encoded_draw_count == 4);
    assert(queue_result.queue->completed_submission_count() == 1);
}

void test_runs_scene_based_frame_plan_with_tiles_snapshot_and_cache_invalidation() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::kGraphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(vme::engine::gfx::contracts::SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    vme::engine::tiles::MapRenderSnapshot snapshot {};
    snapshot.tile_batches.push_back(vme::engine::tiles::TileRenderBatch {.tile_id = 1, .drawable_count = 2});
    snapshot.overlay_batches.push_back(vme::engine::tiles::OverlayRenderBatch {.overlay_id = 5, .drawable_count = 1});
    snapshot.style_revision = 1;
    snapshot.pipeline_revision = 1;

    const auto scene = vme::engine::render::build_scene_render_data_from_tiles_snapshot(snapshot);

    vme::engine::render::SceneMaterialPipelineCache cache {};
    cache.invalidate_if_needed(vme::engine::render::CacheInvalidationState {
        .style_revision = snapshot.style_revision,
        .pipeline_revision = snapshot.pipeline_revision,
    });
    cache.resolve_material("background.base", vme::engine::render::BlendMode::kOpaque, false);
    cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);
    cache.resolve_material("overlays.alpha", vme::engine::render::BlendMode::kAlpha, false);

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder);

    const auto result = vme::engine::render::run_basic_frame_scene(
        *instance,
        *device_result.device,
        *queue_result.queue,
        *swapchain_result.swapchain,
        *encoder,
        scene);

    assert(result.ok);
    assert(result.encoded_pass_count == 3);
    assert(result.encoded_draw_count == 4);
    assert(cache.size() == 3);

    snapshot.style_revision = 2;
    cache.invalidate_if_needed(vme::engine::render::CacheInvalidationState {
        .style_revision = snapshot.style_revision,
        .pipeline_revision = snapshot.pipeline_revision,
    });
    assert(cache.size() == 0);
}

void test_rejects_empty_plan() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::kGraphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(vme::engine::gfx::contracts::SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder);

    const auto result = vme::engine::render::run_basic_frame(
        *instance,
        *device_result.device,
        *queue_result.queue,
        *swapchain_result.swapchain,
        *encoder,
        vme::engine::render::FramePlan {});

    assert(!result.ok);
}

}  // namespace

int main() {
    test_runs_basic_frame_path_with_gfx_stubs();
    test_runs_scene_based_frame_plan_with_tiles_snapshot_and_cache_invalidation();
    test_rejects_empty_plan();
    return 0;
}
