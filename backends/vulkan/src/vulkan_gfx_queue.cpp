

#include "vulkan_gfx_queue.hpp"
#include "vulkan_gfx_device.hpp"
#include "vulkan_command_buffer.hpp" // implementação concreta de ICommandBuffer para Vulkan
#include <cassert>
#include <stdexcept>

namespace vme::backends::vulkan {

// -----------------------------------------------------------------------------
// Construtor / Destrutor
// -----------------------------------------------------------------------------

VulkanGfxQueue::VulkanGfxQueue(VulkanGfxDevice& device, VkQueue vk_queue, std::uint32_t queue_family_index)
    : device_(device), vk_queue_(vk_queue), queue_family_index_(queue_family_index) {
    assert(vk_queue_ != VK_NULL_HANDLE);
}

VulkanGfxQueue::~VulkanGfxQueue() {
    // Esperar a GPU terminar antes de destruir? Depende da política de shutdown.
    // Para simplificar, confiamos que o dispositivo será destruído após a fila.
    // Limpamos os semáforos e fences pendentes.
    std::scoped_lock lock(mutex_);
    for (auto& pend : pending_) {
        for (auto sem : pend.wait_semaphores) vkDestroySemaphore(device_.vk_device(), sem, nullptr);
        for (auto sem : pend.signal_semaphores) vkDestroySemaphore(device_.vk_device(), sem, nullptr);
        if (pend.fence) vkDestroyFence(device_.vk_device(), pend.fence, nullptr);
    }
    pending_.clear();
}

// -----------------------------------------------------------------------------
// Helpers de Sincronização
// -----------------------------------------------------------------------------

VkSemaphore VulkanGfxQueue::get_or_create_semaphore_for_signal() {
    VkSemaphoreCreateInfo sem_info{};
    sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphore semaphore;
    if (vkCreateSemaphore(device_.vk_device(), &sem_info, nullptr, &semaphore) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }
    return semaphore;
}

bool VulkanGfxQueue::build_sync_objects(const gfx::contracts::SubmitInfo& info,
                                        std::vector<VkSemaphore>& out_wait_semaphores,
                                        std::vector<VkPipelineStageFlags>& out_wait_stages,
                                        std::vector<VkSemaphore>& out_signal_semaphores,
                                        VkFence& out_fence,
                                        PendingSubmission& out_meta) {
    // --- Waits (TimelineWaitInfo) ---
    for (const auto& wait : info.waits) {
        if (!wait.timeline) continue;
        // Timeline suporte: na versão inicial, não implementamos semáforos de timeline.
        // Retornamos erro se o contrato exigir espera em timeline.
        return false;
    }

    // --- Signals (TimelineSignalInfo) ---
    for (const auto& signal : info.signals) {
        if (!signal.timeline) continue;
        // Para cada sinalização, criamos um semáforo binário que será sinalizado.
        VkSemaphore sem = get_or_create_semaphore_for_signal();
        out_signal_semaphores.push_back(sem);
        // Guardamos metadados para posteriormente atualizar o valor da timeline (se suportado)
        out_meta.signal_timelines.emplace_back(signal.timeline, signal.value);
    }

    // --- Fence ---
    if (info.fence_signal.fence) {
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        if (vkCreateFence(device_.vk_device(), &fence_info, nullptr, &out_fence) != VK_SUCCESS) {
            return false;
        }
        out_meta.fence_state = info.fence_signal.fence;
        out_meta.fence_value = info.fence_signal.value;
    }

    // --- Command Buffer ---
    if (info.command_buffer) {
        auto* vk_cmd_buf = dynamic_cast<const VulkanCommandBuffer*>(info.command_buffer);
        if (!vk_cmd_buf) return false;
        out_meta.command_buffer = vk_cmd_buf->vk_command_buffer();
    }

    return true;
}

// -----------------------------------------------------------------------------
// Submissão
// -----------------------------------------------------------------------------

gfx::contracts::QueueSubmitResult VulkanGfxQueue::submit(const gfx::contracts::SubmitInfo& submit_info) {
    std::vector<VkSemaphore> wait_sems, signal_sems;
    std::vector<VkPipelineStageFlags> wait_stages;
    VkFence fence = VK_NULL_HANDLE;
    PendingSubmission meta;

    if (!build_sync_objects(submit_info, wait_sems, wait_stages, signal_sems, fence, meta)) {
        return { gfx::contracts::QueueSubmitErrorCode::kSyncUnresolved,
                 "Timeline waits are not yet implemented" };
    }

    if (meta.command_buffer == VK_NULL_HANDLE) {
        return { gfx::contracts::QueueSubmitErrorCode::kInvalidArgument,
                 "Command buffer is null or invalid" };
    }

    // Monta a estrutura de submissão Vulkan
    VkSubmitInfo vk_submit{};
    vk_submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vk_submit.waitSemaphoreCount = static_cast<uint32_t>(wait_sems.size());
    vk_submit.pWaitSemaphores = wait_sems.data();
    vk_submit.pWaitDstStageMask = wait_stages.data();
    vk_submit.commandBufferCount = 1;
    vk_submit.pCommandBuffers = &meta.command_buffer;
    vk_submit.signalSemaphoreCount = static_cast<uint32_t>(signal_sems.size());
    vk_submit.pSignalSemaphores = signal_sems.data();

    // Submete à fila Vulkan
    {
        std::scoped_lock lock(mutex_); // protege o acesso à VkQueue (não estritamente necessário, mas seguro)
        if (vkQueueSubmit(vk_queue_, 1, &vk_submit, fence) != VK_SUCCESS) {
            return { gfx::contracts::QueueSubmitErrorCode::kInternalFailure,
                     "vkQueueSubmit failed" };
        }
    }

    // Armazena a submissão como pendente (para process_next_submission)
    meta.wait_semaphores = std::move(wait_sems);
    meta.signal_semaphores = std::move(signal_sems);
    meta.fence = fence;

    {
        std::scoped_lock lock(mutex_);
        pending_.push_back(std::move(meta));
    }

    return gfx::contracts::QueueSubmitResult::ok_result();
}

gfx::contracts::QueueSubmitResult VulkanGfxQueue::submit_batch(const gfx::contracts::SubmitBatch& submit_batch) {
    // Para simplificar, submetemos cada item individualmente na ordem.
    // Em uma implementação otimizada, poderíamos agrupar em um único vkQueueSubmit.
    for (const auto& info : submit_batch.submissions) {
        auto result = submit(info);
        if (!result.ok()) return result;
    }
    return gfx::contracts::QueueSubmitResult::ok_result();
}

gfx::contracts::QueueSubmitResult VulkanGfxQueue::process_next_submission() {
    std::scoped_lock lock(mutex_);
    if (pending_.empty()) {
        return { gfx::contracts::QueueSubmitErrorCode::kInvalidArgument,
                 "No pending submissions" };
    }

    auto& meta = pending_.front();

    // Espera pelo fence (se existir) para simular a conclusão da submissão
    if (meta.fence != VK_NULL_HANDLE) {
        vkWaitForFences(device_.vk_device(), 1, &meta.fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(device_.vk_device(), meta.fence, nullptr);
        meta.fence = VK_NULL_HANDLE;
    }

    // Atualiza o estado da fence do contrato
    if (meta.fence_state) {
        meta.fence_state->completed_value = meta.fence_value;
    }

    // Para timelines, atualizamos o valor (se suportado)
    for (auto& [timeline, value] : meta.signal_timelines) {
        if (timeline) {
            timeline->value = value;
        }
    }

    // Libera os semáforos criados
    for (auto sem : meta.wait_semaphores) vkDestroySemaphore(device_.vk_device(), sem, nullptr);
    for (auto sem : meta.signal_semaphores) vkDestroySemaphore(device_.vk_device(), sem, nullptr);

    pending_.pop_front();
    ++completed_count_;

    return gfx::contracts::QueueSubmitResult::ok_result();
}

std::uint64_t VulkanGfxQueue::pending_submission_count() const noexcept {
    std::scoped_lock lock(mutex_);
    return pending_.size();
}

std::uint64_t VulkanGfxQueue::completed_submission_count() const noexcept {
    std::scoped_lock lock(mutex_);
    return completed_count_;
}

} // namespace vme::backends::vulkan