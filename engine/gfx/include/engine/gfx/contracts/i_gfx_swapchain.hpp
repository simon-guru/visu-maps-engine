/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
/**
 * @brief Contrato mínimo de swapchain para aquisição de imagens e apresentação.
 * Decisão de design:
 * - expõe apenas métodos essenciais para manter foco e simplicidade;
 * - não expõe ponteiros ou handles nativos para preservar portabilidade;
 * - métodos retornam códigos de erro para evitar exceções e simplificar o fluxo de controle.
 * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos, mensagens, etc.) sem quebrar o contrato existente.
 * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
namespace vme::engine::gfx::contracts {

/**
 * @brief Descritor de configuração para criação ou recriação de swapchain.
 * Decisão de design:
 * - struct simples para encapsular parâmetros de configuração;
 * - permite evolução futura para incluir parâmetros adicionais sem quebrar o contrato.
 * - a estrutura `SwapchainDesc` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como formatos de imagem, modos de apresentação, etc.) sem quebrar o contrato existente.
 * - a estrutura `SwapchainDesc` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do swapchain conforme necessário.
 */
struct SwapchainDesc {
    std::uint32_t image_count{2}; // Número de imagens na swapchain, deve ser maior que zero.
};

enum class SwapchainErrorCode : std::uint16_t {
    Ok = 0, // Operação bem-sucedida.
    InvalidArgument, // Ex: configuração de swapchain inválida.
    InvalidState, // Ex: swapchain em estado inválido.
    OutOfDate, // Ex: swapchain desatualizada.
};

/**
 * @brief Resultado da aquisição de imagem ou apresentação.
 * Decisão de design:
 * - struct simples para encapsular código de erro e dados relevantes;
 * - método `ok()` para facilitar verificações de sucesso;
 * - método estático `ok_result()` para criar facilmente um resultado de sucesso.
 * - evita o uso de exceções para erros, simplificando o fluxo de controle.
 * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos, mensagens, etc.) sem quebrar o contrato existente.
 * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
*/
struct AcquireImageResult {
    SwapchainErrorCode code{SwapchainErrorCode::Ok}; // Código de erro, padrão é Ok para indicar sucesso.
    std::uint32_t image_index{0}; // Índice da imagem adquirida, válido apenas se o código de erro indicar sucesso.
    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok(void) 
     const noexcept { 
        return code == SwapchainErrorCode::Ok; 
    }
};

/**
 * @brief Resultado da apresentação de uma imagem.
 * Decisão de design:
 * - struct simples para encapsular código de erro;
 * - método `ok()` para facilitar verificações de sucesso;
 * - método estático `ok_result()` para criar facilmente um resultado de sucesso.
 * - evita o uso de exceções para erros, simplificando o fluxo de controle.
 * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos, mensagens, etc.) sem quebrar o contrato existente.
 * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
struct PresentResult {
    SwapchainErrorCode code{SwapchainErrorCode::Ok}; // Código de erro, padrão é Ok para indicar sucesso.
    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok(void) 
     const noexcept { 
        return code == SwapchainErrorCode::Ok; 
    }
};

/**
 * @brief Contrato mínimo de swapchain para aquisição de imagens e apresentação.
 * Decisão de design:
 * - expõe apenas métodos essenciais para manter foco e simplicidade;
 * - não expõe ponteiros ou handles nativos para preservar portabilidade;
 * - métodos retornam códigos de erro para evitar exceções e simplificar o fluxo de controle.
 * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos, mensagens, etc.) sem quebrar o contrato existente.
 * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
class IGfxSwapchain {
public:
    virtual ~IGfxSwapchain() = default;

    virtual AcquireImageResult acquire_next_image() = 0;
    virtual PresentResult present(std::uint32_t image_index) = 0;
    virtual SwapchainErrorCode recreate(const SwapchainDesc& desc) = 0;
};

}  // namespace vme::engine::gfx::contracts
