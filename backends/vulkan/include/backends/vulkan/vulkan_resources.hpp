

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/resources/interfaces.hpp>
#include "vulkan_gfx_device.hpp"

namespace vme::backends::vulkan {

class VulkanBuffer final : public gfx::resources::Buffer {
public:
    VulkanBuffer(VulkanGfxDevice& device, const gfx::resources::BufferDesc& desc, VkBuffer buffer, VmaAllocation allocation);
    ~VulkanBuffer() override;

    [[nodiscard]] const gfx::resources::BufferDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint64_t size_bytes() const noexcept override { return desc_.size_bytes; }
    [[nodiscard]] VkBuffer vk_buffer() const noexcept { return buffer_; }

private:
    VulkanGfxDevice& device_;
    gfx::resources::BufferDesc desc_;
    VkBuffer buffer_;
    VmaAllocation allocation_;  // usando VMA para alocação de memória
};

// Similar para Texture, Sampler, ShaderModule, ResourceView...
} // namespace vme::backends::vulkan