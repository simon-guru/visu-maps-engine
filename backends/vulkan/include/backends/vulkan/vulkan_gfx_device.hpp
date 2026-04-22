

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/contracts/i_gfx_device.hpp>
#include "vulkan_gfx_instance.hpp"
#include "vulkan_gfx_queue.hpp"
#include "vulkan_gfx_swapchain.hpp"

namespace vme::backends::vulkan {

class VulkanGfxDevice final : public gfx::contracts::IGfxDevice {
public:
    VulkanGfxDevice(VulkanGfxInstance& instance, VkPhysicalDevice physical_device);
    ~VulkanGfxDevice() override;

    gfx::contracts::CreateQueueResult create_queue(gfx::contracts::QueueType type) override;
    gfx::contracts::CreateSwapchainResult create_swapchain(const gfx::contracts::SwapchainDesc& desc) override;

    [[nodiscard]] const gfx::pipeline::DeviceCaps& device_caps() const noexcept override;
    [[nodiscard]] const gfx::resources::ResourceCaps& resource_caps() const noexcept override;

    gfx::pipeline::CreateGraphicsPipelineResult create_graphics_pipeline(
        const gfx::pipeline::GraphicsPipelineDesc& desc) override;
    gfx::pipeline::CreateComputePipelineResult create_compute_pipeline(
        const gfx::pipeline::ComputePipelineDesc& desc) override;

    // Recursos
    gfx::resources::CreateBufferResult create_buffer(const gfx::resources::BufferDesc& desc) override;
    gfx::resources::CreateTextureResult create_texture(const gfx::resources::TextureDesc& desc) override;
    gfx::resources::CreateSamplerResult create_sampler(const gfx::resources::SamplerDesc& desc) override;
    gfx::resources::CreateShaderModuleResult create_shader_module(
        const gfx::resources::ShaderModuleDesc& desc) override;
    gfx::resources::CreateResourceViewFromBufferResult create_resource_view(
        const gfx::resources::ResourceViewDesc& desc, const gfx::resources::Buffer& buffer) override;
    gfx::resources::CreateResourceViewFromTextureResult create_resource_view(
        const gfx::resources::ResourceViewDesc& desc, const gfx::resources::Texture& texture) override;

    // Acesso interno
    [[nodiscard]] VkDevice vk_device() const noexcept { return device_; }
    [[nodiscard]] VkPhysicalDevice vk_physical_device() const noexcept { return physical_device_; }

    [[nodiscard]] std::uint32_t graphics_queue_family() const noexcept { return graphics_queue_family_; }
    [[nodiscard]] std::uint32_t compute_queue_family() const noexcept { return compute_queue_family_; }
    [[nodiscard]] std::uint32_t transfer_queue_family() const noexcept { return transfer_queue_family_; }

private:

    VkShaderModule get_or_create_shader_module(const std::string& name, const std::vector<std::uint8_t>& code);
    VkPipelineLayout create_pipeline_layout(const gfx::pipeline::PipelineLayoutDesc& desc);
    VkRenderPass get_or_create_compatible_render_pass(const gfx::pipeline::GraphicsPipelineDesc& desc);


    VulkanGfxInstance& instance_;
    VkPhysicalDevice physical_device_;
    VkDevice device_ = VK_NULL_HANDLE;
    uint32_t graphics_queue_family_ = UINT32_MAX;
    uint32_t compute_queue_family_ = UINT32_MAX;
    uint32_t transfer_queue_family_ = UINT32_MAX;
    gfx::pipeline::DeviceCaps caps_;
    gfx::resources::ResourceCaps resource_caps_;

    void find_queue_families();
    void create_logical_device();
};

} // namespace vme::backends::vulkan