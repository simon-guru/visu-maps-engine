#include "vulkan_command_encoder.hpp"
#include "vulkan_command_buffer.hpp"
#include <cassert>
#include <stdexcept>

namespace vme::backends::vulkan {

VulkanCommandEncoder::VulkanCommandEncoder(VulkanGfxDevice& device) : device_(device) {
    // Cria um pool de comandos para a fila gráfica
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = device_.graphics_queue_family(); // método a ser adicionado em VulkanGfxDevice

    if (vkCreateCommandPool(device_.vk_device(), &pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCommandEncoder: failed to create command pool");
    }

    begin_recording();
}

VulkanCommandEncoder::~VulkanCommandEncoder() {
    if (command_pool_ != VK_NULL_HANDLE) {
        // O VkCommandBuffer alocado será liberado automaticamente com o pool
        vkDestroyCommandPool(device_.vk_device(), command_pool_, nullptr);
    }
}

void VulkanCommandEncoder::begin_recording() {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device_.vk_device(), &alloc_info, &command_buffer_) != VK_SUCCESS) {
        throw std::runtime_error("VulkanCommandEncoder: failed to allocate command buffer");
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer_, &begin_info);
    is_recording_ = true;
    recorded_commands_ = 0;
}

gfx::commands::CommandError VulkanCommandEncoder::check_recording() const {
    if (!is_recording_) {
        return { gfx::commands::CommandErrorCode::InvalidState, "encoder is already finished" };
    }
    return gfx::commands::CommandError::ok_result();
}

gfx::commands::CommandError VulkanCommandEncoder::copy(const gfx::commands::CopyBufferCommand& command) {
    if (auto err = check_recording(); !err.ok()) return err;
    if (command.size_bytes == 0) {
        return { gfx::commands::CommandErrorCode::InvalidArgument, "copy size must be > 0" };
    }

    // Nota: para executar a cópia, precisaríamos dos buffers reais.
    // Nesta fase inicial, apenas validamos e contamos.
    // Futuramente: vkCmdCopyBuffer(command_buffer_, src_buffer, dst_buffer, 1, &region);
    ++recorded_commands_;
    return gfx::commands::CommandError::ok_result();
}

gfx::commands::CommandError VulkanCommandEncoder::dispatch(const gfx::commands::DispatchCommand& command) {
    if (auto err = check_recording(); !err.ok()) return err;
    if (command.group_count_x == 0 || command.group_count_y == 0 || command.group_count_z == 0) {
        return { gfx::commands::CommandErrorCode::InvalidArgument, "dispatch group counts must be > 0" };
    }

    // vkCmdDispatch(command_buffer_, command.group_count_x, command.group_count_y, command.group_count_z);
    ++recorded_commands_;
    return gfx::commands::CommandError::ok_result();
}

gfx::commands::CommandError VulkanCommandEncoder::draw(const gfx::commands::DrawCommand& command) {
    if (auto err = check_recording(); !err.ok()) return err;
    if (command.vertex_count == 0 || command.instance_count == 0) {
        return { gfx::commands::CommandErrorCode::InvalidArgument, "vertex/instance count must be > 0" };
    }

    // vkCmdDraw(command_buffer_, command.vertex_count, command.instance_count,
    //           command.first_vertex, command.first_instance);
    ++recorded_commands_;
    return gfx::commands::CommandError::ok_result();
}

gfx::commands::CommandError VulkanCommandEncoder::barrier(const gfx::commands::BarrierCommand& command) {
    if (auto err = check_recording(); !err.ok()) return err;
    if (command.src_stage == command.dst_stage) {
        return { gfx::commands::CommandErrorCode::InvalidArgument, "src and dst stages must differ" };
    }

    // Mapeamento de PipelineStage para VkPipelineStageFlags seria feito aqui.
    // vkCmdPipelineBarrier(...);
    ++recorded_commands_;
    return gfx::commands::CommandError::ok_result();
}

gfx::commands::FinishCommandBufferResult VulkanCommandEncoder::finish(const gfx::commands::CommandBufferDesc& desc) {
    if (!is_recording_) {
        return { nullptr, { gfx::commands::CommandErrorCode::InvalidState, "encoder already finished" } };
    }

    if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
        return { nullptr, { gfx::commands::CommandErrorCode::BackendFailure, "failed to end command buffer" } };
    }

    is_recording_ = false;

    auto cmd_buffer = std::make_unique<VulkanCommandBuffer>(command_buffer_, desc, recorded_commands_);
    return { std::move(cmd_buffer), gfx::commands::CommandError::ok_result() };
}

} // namespace vme::backends::vulkan