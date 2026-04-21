#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"
#include "engine/render/basic_frame_renderer.hpp"
#include "engine/render/material_cache_invalidation_bridge.hpp"
#include "engine/render/material_pipeline_cache.hpp"
#include "engine/render/scene_data_adapter.hpp"
#include "engine/services/theme_style_events.hpp"
#include "engine/tiles/render_snapshot_cache.hpp"

namespace {

void test_runs_basic_frame_path_with_gfx_stubs() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::Graphics);
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

void test_runs_scene_based_frame_from_tiles_cache_and_service_event_invalidation() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::Graphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(vme::engine::gfx::contracts::SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    vme::engine::tiles::RenderSnapshotCache snapshot_cache {};
    snapshot_cache.put_snapshot(vme::engine::tiles::MapRenderSnapshot {
        .render_background = true,
        .tile_batches = {vme::engine::tiles::TileRenderBatch {.tile_id = 1, .drawable_count = 2}},
        .overlay_batches = {vme::engine::tiles::OverlayRenderBatch {.overlay_id = 5, .drawable_count = 1}},
        .style_revision = 1,
        .pipeline_revision = 1,
    });

    assert(snapshot_cache.has_snapshot());
    const auto snapshot = snapshot_cache.latest_snapshot();
    assert(snapshot.has_value());

    const auto scene = vme::engine::render::build_scene_render_data_from_tiles_snapshot(*snapshot);

    vme::engine::render::SceneMaterialPipelineCache cache {};
    vme::engine::render::apply_theme_style_event_to_material_cache(
        cache,
        vme::engine::services::ThemeStyleChangedEvent {
            .style_revision = snapshot->style_revision,
            .pipeline_revision = snapshot->pipeline_revision,
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

    vme::engine::render::apply_theme_style_event_to_material_cache(
        cache,
        vme::engine::services::ThemeStyleChangedEvent {.style_revision = 2, .pipeline_revision = 1});
    assert(cache.size() == 0);
}

}  // namespace

int main() {
    test_runs_basic_frame_path_with_gfx_stubs();
    test_runs_scene_based_frame_from_tiles_cache_and_service_event_invalidation();
    return 0;
}
