


#include "vulkan_gfx_instance.hpp"
#include "vulkan_gfx_device.hpp"
#include <stdexcept>
#include <cstring>

namespace vme::backends::vulkan {

VulkanGfxInstance::VulkanGfxInstance() {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "VME Vulkan Backend";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    std::vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME };
#ifdef VK_USE_PLATFORM_XLIB_KHR
    extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
    // Adicionar outras extensões de plataforma conforme necessário

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&create_info, nullptr, &vk_instance_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    enumerate_physical_devices();
}

VulkanGfxInstance::~VulkanGfxInstance() {
    if (vk_instance_) {
        vkDestroyInstance(vk_instance_, nullptr);
    }
}

void VulkanGfxInstance::enumerate_physical_devices() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(vk_instance_, &count, nullptr);
    physical_devices_.resize(count);
    vkEnumeratePhysicalDevices(vk_instance_, &count, physical_devices_.data());
}

std::uint32_t VulkanGfxInstance::adapter_count() const noexcept {
    return static_cast<std::uint32_t>(physical_devices_.size());
}

gfx::contracts::AdapterInfo VulkanGfxInstance::adapter_info(std::uint32_t index) const noexcept {
    if (index >= physical_devices_.size()) {
        return { index, "Invalid" };
    }
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physical_devices_[index], &props);
    return { index, props.deviceName };
}

gfx::contracts::CreateDeviceResult VulkanGfxInstance::create_device(std::uint32_t adapter_index) {
    if (adapter_index >= physical_devices_.size()) {
        return { nullptr, { gfx::contracts::QueueSubmitErrorCode::kInvalidArgument, "Invalid adapter index" } };
    }
    try {
        auto device = std::make_unique<VulkanGfxDevice>(*this, physical_devices_[adapter_index]);
        return { std::move(device), gfx::contracts::QueueSubmitResult::ok_result() };
    } catch (const std::exception& e) {
        return { nullptr, { gfx::contracts::QueueSubmitErrorCode::kInternalFailure, e.what() } };
    }
}

} // namespace