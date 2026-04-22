

#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <engine/gfx/contracts/i_gfx_instance.hpp>

namespace vme::backends::vulkan {

class VulkanGfxInstance final : public gfx::contracts::IGfxInstance {
public:
    VulkanGfxInstance();
    ~VulkanGfxInstance() override;

    [[nodiscard]] std::uint32_t adapter_count() const noexcept override;
    [[nodiscard]] gfx::contracts::AdapterInfo adapter_info(std::uint32_t index) const noexcept override;
    gfx::contracts::CreateDeviceResult create_device(std::uint32_t adapter_index) override;

    // Acesso interno ao VkInstance (para uso por Device, etc.)
    [[nodiscard]] VkInstance vk_instance() const noexcept { return vk_instance_; }

private:
    VkInstance vk_instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    std::vector<VkPhysicalDevice> physical_devices_;

    void setup_debug_messenger();
    void enumerate_physical_devices();
};

} // namespace vme::backends::vulkan
