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
/**
 * @brief Contrato mínimo de dispositivo para criação de filas, swapchain e pipelines.
 * Decisões suportadas:
 * - expõe apenas criação de filas, swapchain e pipelines para manter foco e simplicidade;
 * - expõe caps para permitir validação explícita antes da criação de recursos ou pipelines;
 * - centraliza criação de recursos para que cada backend possa aplicar limites reais e evitar que chamadores manipulem handles nativos, reduzindo divergência entre APIs.
 * - o resultado da criação do dispositivo inclui um campo de erro para evitar exceções e simplificar o fluxo de controle.
 */
namespace vme::engine::gfx::contracts {

/**
 * Decisão de design:
 - enum fechado para manter contrato estável entre implementações;
 - evita propagar códigos de erro específicos de backend para a API pública.
*/
enum class QueueType {
    Graphics, // Inclui operações de renderização e cópia.
    Compute, // Inclui operações de computação e cópia, mas não renderização.
    Transfer, // Inclui apenas operações de cópia, sem renderização ou computação.
};

// Decisão de design:
// - struct simples para encapsular ponteiro e código de erro, evitando exceções e simplificando o fluxo de controle.
// - método `ok()` para facilitar verificações de sucesso.
// - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos ou mensagens) sem quebrar o contrato existente.
struct CreateQueueResult {
    // Em vez de usar um enum separado, podemos usar o ponteiro nulo para indicar falha, simplificando a estrutura.
    std::unique_ptr<IGfxQueue> queue{};
    QueueSubmitResult error{};

    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok() 
     const noexcept { 
        // O resultado é considerado bem-sucedido se o código de erro indicar sucesso e o ponteiro do queue for válido (não nulo).
        return error.ok() && static_cast<bool>(queue); 
    }
};

// Decisão de design:
// - enum fechado para manter contrato estável entre implementações;
// - evita propagar códigos de erro específicos de backend para a API pública.
struct CreateSwapchainResult {
    // Em vez de usar um enum separado, podemos usar o ponteiro nulo para indicar falha, simplificando a estrutura.
    std::unique_ptr<IGfxSwapchain> swapchain{};
    // O campo de erro é inicializado com um valor padrão de sucesso (kOk) para indicar que, por padrão, a criação é considerada bem-sucedida, a menos que o backend retorne um erro específico.
    SwapchainErrorCode error{SwapchainErrorCode::Ok};

    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok() 
     const noexcept { 
        // O resultado é considerado bem-sucedido se o código de erro indicar sucesso (kOk) e o ponteiro do swapchain for válido (não nulo).
        return error == SwapchainErrorCode::Ok && static_cast<bool>(swapchain); 
    }
};

/**
 * @brief Contrato mínimo de dispositivo para criação de filas, swapchain e pipelines.
 * Decisões suportadas:
 * - expõe apenas criação de filas, swapchain e pipelines para manter foco e simplicidade;
 * - expõe caps para permitir validação explícita antes da criação de recursos ou pipelines;
 * - centraliza criação de recursos para que cada backend possa aplicar limites reais e evitar que chamadores manipulem handles nativos, reduzindo divergência entre APIs.
 * - o resultado da criação do dispositivo inclui um campo de erro para evitar exceções e simplificar o fluxo de controle.
 */
class IGfxDevice {
public:
    virtual ~IGfxDevice() = default;
    // Decisão de design:
    // - métodos de criação retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
    // - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
    // - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
    virtual CreateQueueResult create_queue(QueueType queue_type) = 0;

    // Decisão de design:
    // - a criação da swapchain é responsabilidade do dispositivo, pois pode depender de recursos e capacidades específicas do dispositivo, permitindo que cada backend aplique limites reais e otimize a criação da swapchain de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
    virtual CreateSwapchainResult create_swapchain(const SwapchainDesc& desc) = 0;

    // Decisão de design:
    // - expõe caps para permitir validação explícita antes da criação de recursos ou pipelines, promovendo um estilo de código mais robusto e evitando erros silenciosos durante a criação de recursos ou pipelines que não são suportados pelo backend, além de permitir que a implementação de backend otimize a forma como as capacidades são expostas e validadas, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.
    [[nodiscard]] virtual const pipeline::DeviceCaps& device_caps() const noexcept = 0;

    // Decisão de design:
    // - expõe caps para permitir validação explícita antes da criação de recursos ou pipelines, promovendo um estilo de código mais robusto e evitando erros silenciosos durante a criação de recursos ou pipelines que não são suportados pelo backend, além de permitir que a implementação de backend otimize a forma como as capacidades são expostas e validadas, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.
    [[nodiscard]] virtual const resources::ResourceCaps& resource_caps() const noexcept = 0;

    // Decisão de design:
    // - métodos de criação retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
    // - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
    // - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
    virtual pipeline::CreateGraphicsPipelineResult create_graphics_pipeline(const pipeline::GraphicsPipelineDesc& desc) = 0;

    // Decisão de design:
    // - métodos de criação retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
    // - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
    // - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
    virtual pipeline::CreateComputePipelineResult create_compute_pipeline(const pipeline::ComputePipelineDesc& desc) = 0;

    /**
     * @brief Criação de recursos com validação explícita por contrato/capacidade.
     *
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     */
    virtual resources::CreateBufferResult create_buffer(const resources::BufferDesc& desc) = 0;

    /**
     * @brief Criação de texturas com validação explícita por contrato/capacidade.
     *
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     * - a criação de recursos é centralizada no dispositivo para que cada backend possa aplicar limites reais e otimizar a criação de recursos de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
     * - os métodos de criação de recursos retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
     * - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
     * - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
     */
    virtual resources::CreateTextureResult create_texture(const resources::TextureDesc& desc) = 0;

    /**
     * @brief Criação de recursos com validação explícita por contrato/capacidade.
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     * - a criação de recursos é centralizada no dispositivo para que cada backend possa aplicar limites reais e otimizar a criação de recursos de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
     * - os métodos de criação de recursos retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
     * - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
     * - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
     */
    virtual resources::CreateSamplerResult create_sampler(const resources::SamplerDesc& desc) = 0;

    /**
     * @brief Criação de módulos de shader com validação explícita por contrato/capacidade.
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     * - a criação de recursos é centralizada no dispositivo para que cada backend possa aplicar limites reais e otimizar a criação de recursos de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
     * - os métodos de criação de recursos retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
     * - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
     * - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
     */
    virtual resources::CreateShaderModuleResult create_shader_module(const resources::ShaderModuleDesc& desc) = 0;

    /**
     * @brief Criação de views de recursos com validação explícita por contrato/capacidade.
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     * - a criação de recursos é centralizada no dispositivo para que cada backend possa aplicar limites reais e otimizar a criação de recursos de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
     * - os métodos de criação de recursos retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
     * - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
     * - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
     */
    virtual resources::CreateResourceViewFromBufferResult create_resource_view(const resources::ResourceViewDesc& desc, const resources::Buffer& buffer) = 0;

    /**
     * @brief Criação de views de recursos com validação explícita por contrato/capacidade.
     * Decisão de design:
     * - centralizar criação no device permite que cada backend aplique limites reais;
     * - evita que chamadores manipulem handles nativos e reduz divergência entre APIs.
     * - a criação de recursos é centralizada no dispositivo para que cada backend possa aplicar limites reais e otimizar a criação de recursos de acordo com suas características, além de evitar que chamadores manipulem handles nativos e reduzir divergência entre APIs.
     * - os métodos de criação de recursos retornam structs de resultado que encapsulam tanto o objeto criado quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
     * - as estruturas de resultado incluem métodos auxiliares `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
     * - as estruturas de resultado também incluem métodos estáticos `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
     */
    virtual resources::CreateResourceViewFromTextureResult create_resource_view(const resources::ResourceViewDesc& desc, const resources::Texture& texture) = 0;
};

}  // namespace vme::engine::gfx::contracts
