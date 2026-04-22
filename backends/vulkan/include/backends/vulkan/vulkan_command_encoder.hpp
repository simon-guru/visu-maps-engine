/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 23/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/commands/i_command_encoder.hpp>
#include "vulkan_gfx_device.hpp"

namespace vme::backends::vulkan {

/**
 * @brief Implementação Vulkan de ICommandEncoder.
 *
 * Aloca um VkCommandBuffer e grava comandos abstratos.
 */
class VulkanCommandEncoder final : public gfx::commands::ICommandEncoder {
public:
    explicit VulkanCommandEncoder(VulkanGfxDevice& device);
    ~VulkanCommandEncoder() override;

    gfx::commands::CommandError copy(const gfx::commands::CopyBufferCommand& command) override;
    gfx::commands::CommandError dispatch(const gfx::commands::DispatchCommand& command) override;
    gfx::commands::CommandError draw(const gfx::commands::DrawCommand& command) override;
    gfx::commands::CommandError barrier(const gfx::commands::BarrierCommand& command) override;
    gfx::commands::FinishCommandBufferResult finish(const gfx::commands::CommandBufferDesc& desc) override;

private:
    VulkanGfxDevice& device_;
    VkCommandPool command_pool_ = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;
    bool is_recording_ = true;
    std::uint64_t recorded_commands_ = 0;

    void begin_recording();
    [[nodiscard]] gfx::commands::CommandError check_recording() const;
};

} // namespace vme::backends::vulkan