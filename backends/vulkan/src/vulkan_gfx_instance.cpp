

#include "vulkan_gfx_instance.hpp"
#include <stdexcept>

namespace vme::backends::vulkan {

VulkanGfxInstance::VulkanGfxInstance() {
    // Inicialização mínima para testes
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "VME Vulkan Backend";
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    if (vkCreateInstance(&create_info, nullptr, &vk_instance_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

VulkanGfxInstance::~VulkanGfxInstance() {
    if (vk_instance_) {
        vkDestroyInstance(vk_instance_, nullptr);
    }
}

std::uint32_t VulkanGfxInstance::adapter_count() const noexcept {
    // Stub: retorna 1 para testes
    return 1;
}

gfx::contracts::AdapterInfo VulkanGfxInstance::adapter_info(std::uint32_t index) const noexcept {
    return { index, "Vulkan Stub Adapter" };
}

gfx::contracts::CreateDeviceResult VulkanGfxInstance::create_device(std::uint32_t /*adapter_index*/) {
    // Stub: retorna erro de não implementado por enquanto
    return { nullptr, gfx::contracts::QueueSubmitResult{ gfx::contracts::QueueSubmitErrorCode::kInternalFailure, "Not implemented" } };
}

std::unique_ptr<gfx::contracts::IGfxInstance> create_vulkan_instance() {
    return std::make_unique<VulkanGfxInstance>();
}

} // namespace