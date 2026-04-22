/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/commands/command_common.hpp"
#include "engine/gfx/commands/i_command_buffer.hpp"

namespace vme::engine::gfx::commands {

/**
 * @brief Resultado de finalização do encoder.
 */
struct FinishCommandBufferResult {
    std::unique_ptr<ICommandBuffer> command_buffer{};
    CommandError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(command_buffer); }
};

/**
 * @brief Contrato público para gravação de comandos.
 *
 * Decisões suportadas:
 * - encoder é mutável por design durante a gravação;
 * - ordem de chamada define ordem de execução no command buffer final;
 * - ausência de comandos implícitos/mágicos para garantir portabilidade.
 */
class ICommandEncoder {
public:
    // Destrutor virtual para permitir destruição polimórfica.
    virtual ~ICommandEncoder() = default;

    /**
     * @brief Grava operação de cópia.
     * Decisão de design:
     * - validações de estado e argumentos para garantir contrato consistente;
     * - evita efeitos colaterais e erros silenciosos durante gravação.
     * - a validação de estado garante que comandos não possam ser gravados após o encoder ter sido finalizado, promovendo segurança e previsibilidade na gravação de comandos.
     * - as validações de argumentos garantem que os comandos gravados sejam válidos e possam ser executados corretamente pelos backends gráficos, evitando erros silenciosos e facilitando a depuração.
     * - a implementação de backend pode otimizar a gravação e execução dos comandos, desde que as validações de estado e argumentos sejam mantidas, permitindo melhor desempenho sem sacrificar a correção.
     */
    virtual CommandError copy(const CopyBufferCommand& command) = 0;

    /**
     * @brief Grava operação de compute dispatch.
     * Decisão de design:
     * - validações de estado e argumentos para garantir contrato consistente;
     * - evita efeitos colaterais e erros silenciosos durante gravação.
     * - a validação de estado garante que comandos não possam ser gravados após o encoder ter sido finalizado, promovendo segurança e previsibilidade na gravação de comandos.
     * - as validações de argumentos garantem que os comandos gravados sejam válidos e possam ser executados corretamente pelos backends gráficos, evitando erros silenciosos e facilitando a depuração.
     * - a implementação de backend pode otimizar a gravação e execução dos comandos, desde que as validações de estado e argumentos sejam mantidas, permitindo melhor desempenho sem sacrificar a correção.
     */
    virtual CommandError dispatch(const DispatchCommand& command) = 0;

    /**
     * @brief Grava operação de draw.
     * Decisão de design:
     * - validações de estado e argumentos para garantir contrato consistente;
     * - evita efeitos colaterais e erros silenciosos durante gravação.
     * - a validação de estado garante que comandos não possam ser gravados após o encoder ter sido finalizado, promovendo segurança e previsibilidade na gravação de comandos.
     * - as validações de argumentos garantem que os comandos gravados sejam válidos e possam ser executados corretamente pelos backends gráficos, evitando erros silenciosos e facilitando a depuração.
     * - a implementação de backend pode otimizar a gravação e execução dos comandos, desde que as validações de estado e argumentos sejam mantidas, permitindo melhor desempenho sem sacrificar a correção.
     */
    virtual CommandError draw(const DrawCommand& command) = 0;

    /**
     * @brief Grava operação de barreira/sincronização.
     * Decisão de design:
     * - validações de estado e argumentos para garantir contrato consistente;
     * - evita efeitos colaterais e erros silenciosos durante gravação.
     * - a validação de estado garante que comandos não possam ser gravados após o encoder ter sido finalizado, promovendo segurança e previsibilidade na gravação de comandos.
     * - as validações de argumentos garantem que os comandos gravados sejam válidos e possam ser executados corretamente pelos backends gráficos, evitando erros silenciosos e facilitando a depuração.
     * - a implementação de backend pode otimizar a gravação e execução dos comandos, desde que as validações de estado e argumentos sejam mantidas, permitindo melhor desempenho sem sacrificar a correção.    
     */
    virtual CommandError barrier(const BarrierCommand& command) = 0;

    /**
     * @brief Finaliza gravação e retorna command buffer imutável.
     * Decisão de design:
     * - validações de estado e argumentos para garantir contrato consistente;
     * - separa fase de construção (encoder mutável) da fase de execução (command buffer imutável).
     * - a validação de estado garante que o encoder não possa ser finalizado mais de uma vez, promovendo segurança e previsibilidade na finalização do command buffer.
     */
    virtual FinishCommandBufferResult finish(const CommandBufferDesc& desc) = 0;
};

}  // namespace vme::engine::gfx::commands
