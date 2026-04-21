#include <cassert>

#include "engine/gfx/pipeline.hpp"

namespace {

using vme::engine::gfx::pipeline::ComputePipelineDesc;
using vme::engine::gfx::pipeline::GraphicsPipelineDesc;
using vme::engine::gfx::pipeline::PipelineLayoutDesc;
using vme::engine::gfx::pipeline::PushConstantRangeDesc;
using vme::engine::gfx::pipeline::ResourceSlotDesc;
using vme::engine::gfx::pipeline::ShaderStage;
using vme::engine::gfx::pipeline::ShaderStageDesc;
using vme::engine::gfx::pipeline::validate_compute_pipeline_desc;
using vme::engine::gfx::pipeline::validate_graphics_pipeline_desc;

PipelineLayoutDesc valid_layout() {
    PipelineLayoutDesc layout{};
    layout.resource_slots.push_back(ResourceSlotDesc {
        .set = 0,
        .binding = 0,
        .visibility = {ShaderStage::kVertex, ShaderStage::kFragment},
    });
    layout.push_constants.push_back(PushConstantRangeDesc {
        .offset_bytes = 0,
        .size_bytes = 64,
        .visibility = {ShaderStage::kVertex},
    });
    return layout;
}

void test_graphics_pipeline_validation_accepts_valid_contract() {
    GraphicsPipelineDesc desc{};
    desc.layout = valid_layout();
    desc.shader_stages = {
        ShaderStageDesc {.stage = ShaderStage::kVertex, .entry_point = "main", .module_name = "vs_main"},
        ShaderStageDesc {.stage = ShaderStage::kFragment, .entry_point = "main", .module_name = "fs_main"},
    };

    assert(validate_graphics_pipeline_desc(desc).ok());
}

void test_graphics_pipeline_validation_rejects_compute_stage() {
    GraphicsPipelineDesc desc{};
    desc.layout = valid_layout();
    desc.shader_stages = {
        ShaderStageDesc {.stage = ShaderStage::kVertex, .entry_point = "main", .module_name = "vs_main"},
        ShaderStageDesc {.stage = ShaderStage::kCompute, .entry_point = "main", .module_name = "cs_main"},
    };

    assert(!validate_graphics_pipeline_desc(desc).ok());
}

void test_compute_pipeline_validation_rejects_non_compute_stage() {
    ComputePipelineDesc desc{};
    desc.layout = valid_layout();
    desc.stage = ShaderStageDesc {.stage = ShaderStage::kFragment,
                                  .entry_point = "main",
                                  .module_name = "not_compute"};

    assert(!validate_compute_pipeline_desc(desc).ok());
}

void test_compute_pipeline_validation_accepts_compute_stage() {
    ComputePipelineDesc desc{};
    desc.layout = valid_layout();
    desc.stage = ShaderStageDesc {.stage = ShaderStage::kCompute,
                                  .entry_point = "main",
                                  .module_name = "cs_main"};

    assert(validate_compute_pipeline_desc(desc).ok());
}

}  // namespace

int main() {
    test_graphics_pipeline_validation_accepts_valid_contract();
    test_graphics_pipeline_validation_rejects_compute_stage();
    test_compute_pipeline_validation_rejects_non_compute_stage();
    test_compute_pipeline_validation_accepts_compute_stage();
    return 0;
}
