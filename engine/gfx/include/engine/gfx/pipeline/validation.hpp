/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <algorithm>
#include <string>

#include "engine/gfx/pipeline/pipeline_desc.hpp"

namespace vme::engine::gfx::pipeline {

enum class PipelineErrorCode : std::uint16_t {
    kOk = 0,
    kInvalidArgument,
    kInvalidState,
    kCapabilityNotSupported,
};

struct PipelineError {
    PipelineErrorCode code{PipelineErrorCode::kOk};
    std::string message{};

    [[nodiscard]] bool ok() const noexcept { return code == PipelineErrorCode::kOk; }

    static PipelineError ok_result() noexcept { return {}; }
};

[[nodiscard]] inline PipelineError validate_layout_desc(const PipelineLayoutDesc& desc) {
    for (const auto& slot : desc.resource_slots) {
        if (slot.visibility.empty()) {
            return {PipelineErrorCode::kInvalidArgument,
                    "resource slot visibility must declare at least one stage"};
        }
    }

    for (const auto& range : desc.push_constants) {
        if (range.size_bytes == 0) {
            return {PipelineErrorCode::kInvalidArgument, "push constant size must be greater than zero"};
        }
        if (range.visibility.empty()) {
            return {PipelineErrorCode::kInvalidArgument,
                    "push constant visibility must declare at least one stage"};
        }
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_shader_stage_desc(const ShaderStageDesc& stage_desc) {
    if (stage_desc.module_name.empty()) {
        return {PipelineErrorCode::kInvalidArgument, "shader module_name cannot be empty"};
    }
    if (stage_desc.entry_point.empty()) {
        return {PipelineErrorCode::kInvalidArgument, "shader entry_point cannot be empty"};
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_graphics_pipeline_desc(const GraphicsPipelineDesc& desc) {
    const auto layout_error = validate_layout_desc(desc.layout);
    if (!layout_error.ok()) {
        return layout_error;
    }

    if (desc.shader_stages.empty()) {
        return {PipelineErrorCode::kInvalidArgument, "graphics pipeline requires shader stages"};
    }

    auto has_vertex = false;
    auto has_fragment = false;

    for (const auto& stage : desc.shader_stages) {
        const auto stage_error = validate_shader_stage_desc(stage);
        if (!stage_error.ok()) {
            return stage_error;
        }

        if (stage.stage == ShaderStage::kCompute) {
            return {PipelineErrorCode::kInvalidArgument,
                    "graphics pipeline cannot declare compute shader stage"};
        }

        has_vertex = has_vertex || stage.stage == ShaderStage::kVertex;
        has_fragment = has_fragment || stage.stage == ShaderStage::kFragment;
    }

    if (!has_vertex || !has_fragment) {
        return {PipelineErrorCode::kInvalidArgument,
                "graphics pipeline requires both vertex and fragment stages"};
    }

    return PipelineError::ok_result();
}

[[nodiscard]] inline PipelineError validate_compute_pipeline_desc(const ComputePipelineDesc& desc) {
    const auto layout_error = validate_layout_desc(desc.layout);
    if (!layout_error.ok()) {
        return layout_error;
    }

    const auto stage_error = validate_shader_stage_desc(desc.stage);
    if (!stage_error.ok()) {
        return stage_error;
    }

    if (desc.stage.stage != ShaderStage::kCompute) {
        return {PipelineErrorCode::kInvalidArgument,
                "compute pipeline requires compute shader stage"};
    }

    return PipelineError::ok_result();
}

}  // namespace vme::engine::gfx::pipeline
