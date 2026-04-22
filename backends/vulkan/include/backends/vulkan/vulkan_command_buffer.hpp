/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 23/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/commands/i_command_buffer.hpp>

namespace vme::backends::vulkan {

/**
 * @brief Implementação Vulkan de ICommandBuffer.
 *
 * Encapsula um VkCommandBuffer e seus metadados.
 */
class VulkanCommandBuffer final : public gfx::commands::ICommandBuffer {
public:
    /**
     * @param vk_cmd Command buffer Vulkan já gravado e finalizado.
     * @param desc Descritor usado na finalização.
     * @param command_count Número de comandos gravados.
     */
    VulkanCommandBuffer(VkCommandBuffer vk_cmd,
                        const gfx::commands::CommandBufferDesc& desc,
                        std::uint64_t command_count);
    ~VulkanCommandBuffer() override = default;

    [[nodiscard]] const gfx::commands::CommandBufferDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint64_t command_count() const noexcept override { return command_count_; }

    [[nodiscard]] VkCommandBuffer vk_command_buffer() const noexcept { return vk_cmd_; }

private:
    VkCommandBuffer vk_cmd_;
    gfx::commands::CommandBufferDesc desc_;
    std::uint64_t command_count_;
};

} // namespace vme::backends::vulkan