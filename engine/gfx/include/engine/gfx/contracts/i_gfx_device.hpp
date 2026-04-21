/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/contracts/i_gfx_queue.hpp"
#include "engine/gfx/contracts/i_gfx_swapchain.hpp"
#include "engine/gfx/pipeline/validation.hpp"
#include "engine/gfx/resources/factory.hpp"

namespace vme::engine::gfx::contracts {

enum class QueueType {
    Graphics,
    Compute,
    Transfer,
};

struct CreateQueueResult {
    std::unique_ptr<IGfxQueue> queue{};
    QueueSubmitResult error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(queue); }
};

struct CreateSwapchainResult {
    std::unique_ptr<IGfxSwapchain> swapchain{};
    SwapchainErrorCode error{SwapchainErrorCode::kOk};

    [[nodiscard]] bool ok() const noexcept {
        return error == SwapchainErrorCode::kOk && static_cast<bool>(swapchain);
    }
};

/**
 * @brief Contrato mínimo de dispositivo para criação de filas, swapchain e pipelines.
 */
class IGfxDevice {
public:
    virtual ~IGfxDevice() = default;

    virtual CreateQueueResult create_queue(QueueType queue_type) = 0;
    virtual CreateSwapchainResult create_swapchain(const SwapchainDesc& desc) = 0;

    [[nodiscard]] virtual const pipeline::DeviceCaps& device_caps() const noexcept = 0;
    [[nodiscard]] virtual const resources::ResourceCaps& resource_caps() const noexcept = 0;

    virtual pipeline::CreateGraphicsPipelineResult
    create_graphics_pipeline(const pipeline::GraphicsPipelineDesc& desc) = 0;

    virtual pipeline::CreateComputePipelineResult
    create_compute_pipeline(const pipeline::ComputePipelineDesc& desc) = 0;

    /**
     * @brief Criação de recursos com validação explícita por contrato/capacidade.
     *
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     */
    virtual resources::CreateBufferResult create_buffer(const resources::BufferDesc& desc) = 0;
    virtual resources::CreateTextureResult create_texture(const resources::TextureDesc& desc) = 0;
    virtual resources::CreateSamplerResult create_sampler(const resources::SamplerDesc& desc) = 0;
    virtual resources::CreateShaderModuleResult create_shader_module(
        const resources::ShaderModuleDesc& desc) = 0;
    virtual resources::CreateResourceViewFromBufferResult create_resource_view(
        const resources::ResourceViewDesc& desc, const resources::Buffer& buffer) = 0;
    virtual resources::CreateResourceViewFromTextureResult create_resource_view(
        const resources::ResourceViewDesc& desc, const resources::Texture& texture) = 0;
};

}  // namespace vme::engine::gfx::contracts
