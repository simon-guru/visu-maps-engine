/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>

#include "engine/gfx/pipeline/pipeline_desc.hpp"

namespace vme::engine::gfx::pipeline {

enum class PipelineErrorCode : std::uint16_t {
    Ok = 0,
    InvalidArgument,
    InvalidState,
    CapabilityNotSupported,
};

struct PipelineError {
    PipelineErrorCode code{PipelineErrorCode::Ok};
    std::string message{};

    [[nodiscard]] bool ok() const noexcept { return code == PipelineErrorCode::Ok; }

    static PipelineError ok_result() noexcept { return {}; }
};

struct CreateGraphicsPipelineResult {
    std::uint64_t pipeline_id{0};
    bool cache_hit{false};
    PipelineError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && pipeline_id != 0; }
};

struct CreateComputePipelineResult {
    std::uint64_t pipeline_id{0};
    bool cache_hit{false};
    PipelineError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && pipeline_id != 0; }
};

[[nodiscard]] inline PipelineError validate_layout_desc(const PipelineLayoutDesc& desc) {
    for (const auto& slot : desc.resource_slots) {
        if (slot.visibility.empty()) {
            return {PipelineErrorCode::InvalidArgument,
                    "resource slot visibility must declare at least one stage"};
        }
    }

    for (const auto& range : desc.push_constants) {
        if (range.size_bytes == 0) {
            return {PipelineErrorCode::InvalidArgument, "push constant size must be greater than zero"};
        }
        if (range.visibility.empty()) {
            return {PipelineErrorCode::InvalidArgument,
                    "push constant visibility must declare at least one stage"};
        }
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_shader_stage_desc(const ShaderStageDesc& stage_desc) {
    if (stage_desc.module_name.empty()) {
        return {PipelineErrorCode::InvalidArgument, "shader module_name cannot be empty"};
    }
    if (stage_desc.entry_point.empty()) {
        return {PipelineErrorCode::InvalidArgument, "shader entry_point cannot be empty"};
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline bool is_valid_sample_count(std::uint8_t sample_count) {
    return sample_count == 1 || sample_count == 2 || sample_count == 4 || sample_count == 8;
}

[[nodiscard]] inline PipelineError validate_graphics_pipeline_desc(const GraphicsPipelineDesc& desc,
                                                                   const DeviceCaps& caps) {
    const auto layout_error = validate_layout_desc(desc.layout);
    if (!layout_error.ok()) {
        return layout_error;
    }

    if (desc.shader_stages.empty()) {
        return {PipelineErrorCode::InvalidArgument, "graphics pipeline requires shader stages"};
    }

    if (desc.color_attachments.empty()) {
        return {PipelineErrorCode::InvalidArgument,
                "graphics pipeline requires at least one color attachment"};
    }

    if (desc.color_attachments.size() > caps.max_color_attachments) {
        return {PipelineErrorCode::CapabilityNotSupported,
                "graphics pipeline color attachment count exceeds device capabilities"};
    }

    if (!is_valid_sample_count(desc.sample_count)) {
        return {PipelineErrorCode::InvalidArgument,
                "graphics pipeline sample_count must be one of {1,2,4,8}"};
    }

    if (desc.sample_count > caps.max_sample_count) {
        return {PipelineErrorCode::CapabilityNotSupported,
                "graphics pipeline sample_count exceeds device capabilities"};
    }

    if (desc.rasterization.polygon_mode == PolygonMode::Line && !caps.supports_wireframe) {
        return {PipelineErrorCode::CapabilityNotSupported,
                "wireframe polygon mode is not supported by this device"};
    }

    if (desc.rasterization.depth_clamp_enabled && !caps.supports_depth_clamp) {
        return {PipelineErrorCode::CapabilityNotSupported,
                "depth clamp is not supported by this device"};
    }

    auto has_vertex = false;
    auto has_fragment = false;

    for (const auto& stage : desc.shader_stages) {
        const auto stage_error = validate_shader_stage_desc(stage);
        if (!stage_error.ok()) {
            return stage_error;
        }

        if (stage.stage == ShaderStage::Compute) {
            return {PipelineErrorCode::InvalidArgument,
                    "graphics pipeline cannot declare compute shader stage"};
        }

        has_vertex = has_vertex || stage.stage == ShaderStage::Vertex;
        has_fragment = has_fragment || stage.stage == ShaderStage::Fragment;
    }

    if (!has_vertex || !has_fragment) {
        return {PipelineErrorCode::InvalidArgument,
                "graphics pipeline requires both vertex and fragment stages"};
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_graphics_pipeline_desc(const GraphicsPipelineDesc& desc) {
    return validate_graphics_pipeline_desc(desc, DeviceCaps {});
}

[[nodiscard]] inline PipelineError validate_compute_pipeline_desc(const ComputePipelineDesc& desc,
                                                                  const DeviceCaps& caps) {
    if (!caps.supports_compute) {
        return {PipelineErrorCode::CapabilityNotSupported,
                "compute pipeline is not supported by this device"};
    }

    const auto layout_error = validate_layout_desc(desc.layout);
    if (!layout_error.ok()) {
        return layout_error;
    }

    const auto stage_error = validate_shader_stage_desc(desc.stage);
    if (!stage_error.ok()) {
        return stage_error;
    }

    if (desc.stage.stage != ShaderStage::Compute) {
        return {PipelineErrorCode::InvalidArgument,
                "compute pipeline requires compute shader stage"};
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_compute_pipeline_desc(const ComputePipelineDesc& desc) {
    return validate_compute_pipeline_desc(desc, DeviceCaps {});
}

}  // namespace vme::engine::gfx::pipeline
