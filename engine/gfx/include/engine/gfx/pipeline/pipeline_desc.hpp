/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vme::engine::gfx::pipeline {

enum class ShaderStage : std::uint8_t {
    kVertex,
    kFragment,
    kCompute,
};

enum class ResourceType : std::uint8_t {
    kUniformBuffer,
    kStorageBuffer,
    kSampledTexture,
    kStorageTexture,
    kSampler,
};

struct ResourceSlotDesc {
    std::uint32_t set{0};
    std::uint32_t binding{0};
    ResourceType type{ResourceType::kUniformBuffer};
    std::vector<ShaderStage> visibility{};
};

struct PushConstantRangeDesc {
    std::uint32_t offset_bytes{0};
    std::uint32_t size_bytes{0};
    std::vector<ShaderStage> visibility{};
};

struct PipelineLayoutDesc {
    std::string label{};
    std::vector<ResourceSlotDesc> resource_slots{};
    std::vector<PushConstantRangeDesc> push_constants{};
};

struct ShaderStageDesc {
    ShaderStage stage{ShaderStage::kVertex};
    std::string entry_point{"main"};
    std::string module_name{};
};

enum class PrimitiveTopology : std::uint8_t {
    kTriangleList,
    kTriangleStrip,
    kLineList,
    kLineStrip,
    kPointList,
};

enum class PolygonMode : std::uint8_t {
    kFill,
    kLine,
};

enum class CullMode : std::uint8_t {
    kNone,
    kFront,
    kBack,
};

enum class FrontFace : std::uint8_t {
    kCounterClockwise,
    kClockwise,
};

enum class BlendFactor : std::uint8_t {
    kZero,
    kOne,
    kSrcAlpha,
    kOneMinusSrcAlpha,
};

enum class BlendOp : std::uint8_t {
    kAdd,
    kSubtract,
};

struct ColorBlendDesc {
    bool enabled{false};
    BlendFactor src_color{BlendFactor::kOne};
    BlendFactor dst_color{BlendFactor::kZero};
    BlendOp color_op{BlendOp::kAdd};
    BlendFactor src_alpha{BlendFactor::kOne};
    BlendFactor dst_alpha{BlendFactor::kZero};
    BlendOp alpha_op{BlendOp::kAdd};
};

enum class CompareOp : std::uint8_t {
    kNever,
    kLess,
    kEqual,
    kLessOrEqual,
    kGreater,
    kNotEqual,
    kGreaterOrEqual,
    kAlways,
};

struct DepthStencilDesc {
    bool depth_test_enabled{true};
    bool depth_write_enabled{true};
    CompareOp depth_compare{CompareOp::kLessOrEqual};
    bool stencil_test_enabled{false};
};

struct RasterizationDesc {
    PolygonMode polygon_mode{PolygonMode::kFill};
    CullMode cull_mode{CullMode::kBack};
    FrontFace front_face{FrontFace::kCounterClockwise};
    bool depth_clamp_enabled{false};
};

struct GraphicsPipelineDesc {
    std::string label{};
    PipelineLayoutDesc layout{};
    std::vector<ShaderStageDesc> shader_stages{};
    PrimitiveTopology topology{PrimitiveTopology::kTriangleList};
    RasterizationDesc rasterization{};
    ColorBlendDesc blend{};
    DepthStencilDesc depth_stencil{};
};

struct ComputePipelineDesc {
    std::string label{};
    PipelineLayoutDesc layout{};
    ShaderStageDesc stage{};
};

}  // namespace vme::engine::gfx::pipeline
