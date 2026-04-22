

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/contracts/i_gfx_swapchain.hpp>
#include "vulkan_gfx_device.hpp"

namespace vme::backends::vulkan {

class VulkanGfxSwapchain final : public gfx::contracts::IGfxSwapchain {
public:
    VulkanGfxSwapchain(VulkanGfxDevice& device, VkSurfaceKHR surface, const gfx::contracts::SwapchainDesc& desc);
    ~VulkanGfxSwapchain() override;

    gfx::contracts::AcquireImageResult acquire_next_image() override;
    gfx::contracts::PresentResult present(std::uint32_t image_index) override;
    gfx::contracts::SwapchainErrorCode recreate(const gfx::contracts::SwapchainDesc& desc) override;

private:
    VulkanGfxDevice& device_;
    VkSurfaceKHR surface_;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;
    VkFormat image_format_;
    VkExtent2D extent_;
    gfx::contracts::SwapchainDesc desc_;

    void create_swapchain();
    void cleanup_swapchain();
};

} // namespace