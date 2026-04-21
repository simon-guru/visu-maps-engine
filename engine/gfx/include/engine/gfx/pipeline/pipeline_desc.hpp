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
    Vertex,
    Fragment,
    Compute,
};

enum class ResourceType : std::uint8_t {
    UniformBuffer,
    StorageBuffer,
    SampledTexture,
    StorageTexture,
    Sampler,
};

struct ResourceSlotDesc {
    std::uint32_t set{0};
    std::uint32_t binding{0};
    ResourceType type{ResourceType::UniformBuffer};
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
    ShaderStage stage{ShaderStage::Vertex};
    std::string entry_point{"main"};
    std::string module_name{};
};

enum class PrimitiveTopology : std::uint8_t {
    TriangleList,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList,
};

enum class PolygonMode : std::uint8_t {
    Fill,
    Line,
};

enum class CullMode : std::uint8_t {
    None,
    Front,
    Back,
};

enum class FrontFace : std::uint8_t {
    CounterClockwise,
    Clockwise,
};

enum class BlendFactor : std::uint8_t {
    Zero,
    One,
    SrcAlpha,
    OneMinusSrcAlpha,
};

enum class BlendOp : std::uint8_t {
    Add,
    Subtract,
};

struct ColorBlendDesc {
    bool enabled{false};
    BlendFactor src_color{BlendFactor::One};
    BlendFactor dst_color{BlendFactor::Zero};
    BlendOp color_op{BlendOp::Add};
    BlendFactor src_alpha{BlendFactor::One};
    BlendFactor dst_alpha{BlendFactor::Zero};
    BlendOp alpha_op{BlendOp::Add};
};

enum class RenderTargetFormat : std::uint8_t {
    Rgba8Unorm,
    Bgra8Unorm,
    Rgba16Float,
};

struct ColorAttachmentDesc {
    RenderTargetFormat format{RenderTargetFormat::Rgba8Unorm};
    ColorBlendDesc blend{};
};

enum class CompareOp : std::uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

struct DepthStencilDesc {
    bool depth_test_enabled{true};
    bool depth_write_enabled{true};
    CompareOp depth_compare{CompareOp::LessOrEqual};
    bool stencil_test_enabled{false};
};

struct RasterizationDesc {
    PolygonMode polygon_mode{PolygonMode::Fill};
    CullMode cull_mode{CullMode::Back};
    FrontFace front_face{FrontFace::CounterClockwise};
    bool depth_clamp_enabled{false};
};

struct GraphicsPipelineDesc {
    std::string label{};
    PipelineLayoutDesc layout{};
    std::vector<ShaderStageDesc> shader_stages{};
    PrimitiveTopology topology{PrimitiveTopology::TriangleList};
    RasterizationDesc rasterization{};
    std::vector<ColorAttachmentDesc> color_attachments{};
    DepthStencilDesc depth_stencil{};
    std::uint8_t sample_count{1};
};

struct ComputePipelineDesc {
    std::string label{};
    PipelineLayoutDesc layout{};
    ShaderStageDesc stage{.stage = ShaderStage::Compute};
};

struct DeviceCaps {
    bool supports_compute{true};
    bool supports_wireframe{false};
    bool supports_depth_clamp{false};
    std::uint8_t max_color_attachments{4};
    std::uint8_t max_sample_count{1};
};

}  // namespace vme::engine::gfx::pipeline
