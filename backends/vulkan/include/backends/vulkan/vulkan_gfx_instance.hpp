
#pragma once

#include <memory>
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

private:
    VkInstance vk_instance_ = VK_NULL_HANDLE;
    // ... outros membros
};

// Função fábrica para criar a instância
std::unique_ptr<gfx::contracts::IGfxInstance> create_vulkan_instance();

} // namespace