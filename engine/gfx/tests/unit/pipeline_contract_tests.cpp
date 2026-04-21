#include <cassert>

#include "engine/gfx/pipeline.hpp"

namespace {

using vme::engine::gfx::pipeline::ColorAttachmentDesc;
using vme::engine::gfx::pipeline::ComputePipelineDesc;
using vme::engine::gfx::pipeline::DeviceCaps;
using vme::engine::gfx::pipeline::GraphicsPipelineDesc;
using vme::engine::gfx::pipeline::PipelineErrorCode;
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
        .visibility = {ShaderStage::Vertex, ShaderStage::Fragment},
    });
    layout.push_constants.push_back(PushConstantRangeDesc {
        .offset_bytes = 0,
        .size_bytes = 64,
        .visibility = {ShaderStage::Vertex},
    });
    return layout;
}

GraphicsPipelineDesc valid_graphics_desc() {
    GraphicsPipelineDesc desc{};
    desc.layout = valid_layout();
    desc.shader_stages = {
        ShaderStageDesc {.stage = ShaderStage::Vertex, .entry_point = "main", .module_name = "vs_main"},
        ShaderStageDesc {.stage = ShaderStage::Fragment, .entry_point = "main", .module_name = "fs_main"},
    };
    desc.color_attachments = {ColorAttachmentDesc {}};
    desc.sample_count = 1;
    return desc;
}

void test_graphics_pipeline_validation_accepts_valid_contract() {
    const auto desc = valid_graphics_desc();
    assert(validate_graphics_pipeline_desc(desc).ok());
}

void test_graphics_pipeline_validation_rejects_compute_stage() {
    auto desc = valid_graphics_desc();
    desc.shader_stages[1] =
        ShaderStageDesc {.stage = ShaderStage::Compute, .entry_point = "main", .module_name = "cs_main"};

    assert(!validate_graphics_pipeline_desc(desc).ok());
}

void test_graphics_pipeline_validation_rejects_missing_attachment() {
    auto desc = valid_graphics_desc();
    desc.color_attachments.clear();

    assert(!validate_graphics_pipeline_desc(desc).ok());
}

void test_graphics_pipeline_validation_checks_caps_limits() {
    auto desc = valid_graphics_desc();
    desc.sample_count = 4;

    const DeviceCaps caps {.supports_compute = true,
                           .supports_wireframe = false,
                           .supports_depth_clamp = false,
                           .max_color_attachments = 4,
                           .max_sample_count = 2};

    const auto result = validate_graphics_pipeline_desc(desc, caps);
    assert(result.code == PipelineErrorCode::CapabilityNotSupported);
}

void test_compute_pipeline_validation_rejects_non_compute_stage() {
    ComputePipelineDesc desc{};
    desc.layout = valid_layout();
    desc.stage = ShaderStageDesc {.stage = ShaderStage::Fragment,
                                  .entry_point = "main",
                                  .module_name = "not_compute"};

    assert(!validate_compute_pipeline_desc(desc).ok());
}

void test_compute_pipeline_validation_respects_capabilities() {
    ComputePipelineDesc desc{};
    desc.layout = valid_layout();
    desc.stage = ShaderStageDesc {.stage = ShaderStage::Compute,
                                  .entry_point = "main",
                                  .module_name = "cs_main"};

    const DeviceCaps caps {.supports_compute = false,
                           .supports_wireframe = false,
                           .supports_depth_clamp = false,
                           .max_color_attachments = 4,
                           .max_sample_count = 1};

    const auto result = validate_compute_pipeline_desc(desc, caps);
    assert(result.code == PipelineErrorCode::CapabilityNotSupported);
}

}  // namespace

int main() {
    test_graphics_pipeline_validation_accepts_valid_contract();
    test_graphics_pipeline_validation_rejects_compute_stage();
    test_graphics_pipeline_validation_rejects_missing_attachment();
    test_graphics_pipeline_validation_checks_caps_limits();
    test_compute_pipeline_validation_rejects_non_compute_stage();
    test_compute_pipeline_validation_respects_capabilities();
    return 0;
}
