
#pragma once
#include "backends/vulkan/vulkan_gfx_device.hpp"

namespace vme::backends::vulkan {

inline VkShaderStageFlagBits to_vk_shader_stage(gfx::pipeline::ShaderStage stage) {
    switch (stage) {
        case gfx::pipeline::ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
        case gfx::pipeline::ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case gfx::pipeline::ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
    }
    return VK_SHADER_STAGE_VERTEX_BIT;
}

inline VkPrimitiveTopology to_vk_topology(gfx::pipeline::PrimitiveTopology topo) {
    switch (topo) {
        case gfx::pipeline::PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case gfx::pipeline::PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case gfx::pipeline::PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case gfx::pipeline::PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case gfx::pipeline::PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    }
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

// ... to_vk_polygon_mode, to_vk_cull_mode, to_vk_blend_factor, to_vk_blend_op, to_vk_compare_op ...

} // namespace vme::backends::vulkan