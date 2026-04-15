#include <cassert>

#include "engine/render/frame_plan.hpp"
#include "engine/render/material_cache_invalidation_bridge.hpp"
#include "engine/render/material_pipeline_cache.hpp"
#include "engine/render/scene_data_adapter.hpp"
#include "engine/services/theme_style_events.hpp"
#include "engine/tiles/render_snapshot_cache.hpp"

namespace {

void test_default_frame_plan_has_expected_pass_order() {
    const auto plan = vme::engine::render::build_default_frame_plan();
    assert(plan.passes.size() == 3);

    assert(plan.passes[0].pass_type == vme::engine::render::RenderPassType::kBackground);
    assert(plan.passes[1].pass_type == vme::engine::render::RenderPassType::kTiles);
    assert(plan.passes[2].pass_type == vme::engine::render::RenderPassType::kOverlays);
}

void test_builds_frame_plan_from_tiles_snapshot_cache() {
    vme::engine::tiles::RenderSnapshotCache cache {};
    cache.put_snapshot(vme::engine::tiles::MapRenderSnapshot {
        .render_background = true,
        .tile_batches = {
            vme::engine::tiles::TileRenderBatch {.tile_id = 11, .drawable_count = 2},
            vme::engine::tiles::TileRenderBatch {.tile_id = 12, .drawable_count = 1},
        },
        .overlay_batches = {
            vme::engine::tiles::OverlayRenderBatch {.overlay_id = 21, .drawable_count = 3},
        },
    });

    const auto snapshot = cache.latest_snapshot();
    assert(snapshot.has_value());

    const auto scene = vme::engine::render::build_scene_render_data_from_tiles_snapshot(*snapshot);
    const auto plan = vme::engine::render::build_frame_plan_from_scene(scene);
    assert(plan.passes.size() == 3);
    assert(plan.passes[1].draw_calls == 3);
    assert(plan.passes[2].draw_calls == 3);
}

void test_material_cache_invalidation_with_theme_event_bridge() {
    vme::engine::render::SceneMaterialPipelineCache cache {};

    vme::engine::render::apply_theme_style_event_to_material_cache(
        cache,
        vme::engine::services::ThemeStyleChangedEvent {.style_revision = 1, .pipeline_revision = 1});

    cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);
    assert(cache.size() == 1);

    vme::engine::render::apply_theme_style_event_to_material_cache(
        cache,
        vme::engine::services::ThemeStyleChangedEvent {.style_revision = 2, .pipeline_revision = 1});
    assert(cache.size() == 0);
}

}  // namespace

int main() {
    test_default_frame_plan_has_expected_pass_order();
    test_builds_frame_plan_from_tiles_snapshot_cache();
    test_material_cache_invalidation_with_theme_event_bridge();
    return 0;
}
