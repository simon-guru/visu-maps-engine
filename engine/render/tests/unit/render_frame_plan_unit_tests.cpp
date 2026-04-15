#include <cassert>

#include "engine/render/frame_plan.hpp"
#include "engine/render/material_pipeline_cache.hpp"
#include "engine/render/scene_data_adapter.hpp"

namespace {

void test_default_frame_plan_has_expected_pass_order() {
    const auto plan = vme::engine::render::build_default_frame_plan();
    assert(plan.passes.size() == 3);

    assert(plan.passes[0].pass_type == vme::engine::render::RenderPassType::kBackground);
    assert(plan.passes[1].pass_type == vme::engine::render::RenderPassType::kTiles);
    assert(plan.passes[2].pass_type == vme::engine::render::RenderPassType::kOverlays);
}

void test_pass_descriptor_validation_rejects_invalid_material() {
    vme::engine::render::PassDescriptor invalid_pass {};
    invalid_pass.draw_calls = 1;
    invalid_pass.material.material_name = "";

    assert(!vme::engine::render::is_valid_pass_descriptor(invalid_pass));
}

void test_builds_frame_plan_from_tiles_snapshot() {
    vme::engine::tiles::MapRenderSnapshot snapshot {};
    snapshot.tile_batches.push_back(vme::engine::tiles::TileRenderBatch {.tile_id = 11, .drawable_count = 2});
    snapshot.tile_batches.push_back(vme::engine::tiles::TileRenderBatch {.tile_id = 12, .drawable_count = 1});
    snapshot.overlay_batches.push_back(vme::engine::tiles::OverlayRenderBatch {.overlay_id = 21, .drawable_count = 3});

    const auto scene = vme::engine::render::build_scene_render_data_from_tiles_snapshot(snapshot);
    const auto plan = vme::engine::render::build_frame_plan_from_scene(scene);
    assert(plan.passes.size() == 3);
    assert(plan.passes[1].draw_calls == 3);
    assert(plan.passes[2].draw_calls == 3);
}

void test_scene_material_pipeline_cache_reuses_descriptors() {
    vme::engine::render::SceneMaterialPipelineCache cache {};

    const auto& first = cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);
    const auto& second = cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);

    assert(first.material_name == second.material_name);
    assert(cache.size() == 1);
    assert(cache.cache_hits() == 1);
    assert(cache.cache_misses() == 1);
}

void test_cache_invalidation_on_style_or_pipeline_changes() {
    vme::engine::render::SceneMaterialPipelineCache cache {};
    cache.invalidate_if_needed(vme::engine::render::CacheInvalidationState {.style_revision = 1, .pipeline_revision = 1});

    cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);
    assert(cache.size() == 1);

    cache.invalidate_if_needed(vme::engine::render::CacheInvalidationState {.style_revision = 2, .pipeline_revision = 1});
    assert(cache.size() == 0);

    cache.resolve_material("tiles.standard", vme::engine::render::BlendMode::kOpaque, true);
    assert(cache.size() == 1);

    cache.invalidate_if_needed(vme::engine::render::CacheInvalidationState {.style_revision = 2, .pipeline_revision = 2});
    assert(cache.size() == 0);
}

}  // namespace

int main() {
    test_default_frame_plan_has_expected_pass_order();
    test_pass_descriptor_validation_rejects_invalid_material();
    test_builds_frame_plan_from_tiles_snapshot();
    test_scene_material_pipeline_cache_reuses_descriptors();
    test_cache_invalidation_on_style_or_pipeline_changes();
    return 0;
}
