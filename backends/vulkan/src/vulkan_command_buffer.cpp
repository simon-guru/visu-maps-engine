#include "vulkan_command_buffer.hpp"

namespace vme::backends::vulkan {

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer vk_cmd,
                                         const gfx::commands::CommandBufferDesc& desc,
                                         std::uint64_t command_count)
    : vk_cmd_(vk_cmd), desc_(desc), command_count_(command_count) {}

} // namespace vme::backends::vulkan