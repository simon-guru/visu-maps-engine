/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 23/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <vulkan/vulkan.h>
#include <engine/gfx/contracts/i_gfx_queue.hpp>
#include <deque>
#include <mutex>
#include <vector>

namespace vme::backends::vulkan {

// Forward declaration
class VulkanGfxDevice;
class VulkanCommandBuffer; // implementação concreta de ICommandBuffer para Vulkan

/**
 * @brief Implementação Vulkan de IGfxQueue.
 *
 * Gerencia submissões para uma VkQueue, com suporte a fences e semáforos.
 * Mantém uma fila interna de submissões pendentes para simular processamento
 * assíncrono (útil para testes e para emular o comportamento esperado pelo contrato).
 */
class VulkanGfxQueue final : public gfx::contracts::IGfxQueue {
public:
    /**
     * @param device Referência para o dispositivo Vulkan.
     * @param vk_queue Handle da fila Vulkan.
     * @param queue_family_index Índice da família da fila (necessário para criação de command pools).
     */
    VulkanGfxQueue(VulkanGfxDevice& device, VkQueue vk_queue, std::uint32_t queue_family_index);
    ~VulkanGfxQueue() override;

    // --- IGfxQueue ---
    gfx::contracts::QueueSubmitResult submit(const gfx::contracts::SubmitInfo& submit_info) override;
    gfx::contracts::QueueSubmitResult submit_batch(const gfx::contracts::SubmitBatch& submit_batch) override;
    gfx::contracts::QueueSubmitResult process_next_submission() override;
    [[nodiscard]] std::uint64_t pending_submission_count() const noexcept override;
    [[nodiscard]] std::uint64_t completed_submission_count() const noexcept override;

    // Acesso interno
    [[nodiscard]] VkQueue vk_queue() const noexcept { return vk_queue_; }
    [[nodiscard]] std::uint32_t queue_family_index() const noexcept { return queue_family_index_; }

private:
    VulkanGfxDevice& device_;
    VkQueue vk_queue_ = VK_NULL_HANDLE;
    std::uint32_t queue_family_index_ = 0;

    mutable std::mutex mutex_;                     // protege as estruturas internas
    std::deque<PendingSubmission> pending_;        // submissões ainda não processadas
    std::uint64_t completed_count_ = 0;

    // Estrutura que armazena os recursos de sincronização de uma submissão pendente
    struct PendingSubmission {
        std::vector<VkSemaphore> wait_semaphores;
        std::vector<VkPipelineStageFlags> wait_stages;
        std::vector<VkSemaphore> signal_semaphores;
        VkFence fence = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        // Metadados adicionais para mapear de volta aos objetos do contrato (timeline/fence)
        gfx::contracts::FenceState* fence_state = nullptr;
        gfx::contracts::FenceValue fence_value{};
        std::vector<std::pair<gfx::contracts::QueueTimeline*, std::uint64_t>> signal_timelines;
    };

    // Converte as informações de espera/sinalização do contrato para objetos Vulkan.
    // Retorna false em caso de erro (ex.: timeline não suportada).
    bool build_sync_objects(const gfx::contracts::SubmitInfo& info,
                            std::vector<VkSemaphore>& out_wait_semaphores,
                            std::vector<VkPipelineStageFlags>& out_wait_stages,
                            std::vector<VkSemaphore>& out_signal_semaphores,
                            VkFence& out_fence,
                            PendingSubmission& out_meta);

    // Cria um semáforo Vulkan (ou reutiliza um existente) para um determinado valor de timeline.
    // Para simplificar, criamos um novo semáforo binário para cada sinalização.
    [[nodiscard]] VkSemaphore get_or_create_semaphore_for_signal();
};

} // namespace vme::backends::vulkan