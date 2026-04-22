/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

#include "engine/gfx/commands/command_common.hpp"

namespace vme::engine::gfx::commands {

/**
 * @brief Contrato imutável de comandos finalizados.
 *
 * Justificativa:
 * - separa fase de construção (encoder mutável) da fase de execução;
 * - reduz efeitos colaterais e facilita rastreio determinístico.
 * - a imutabilidade do `ICommandBuffer` finalizado garante que os comandos gravados não possam ser modificados após a finalização, promovendo segurança e previsibilidade na execução dos comandos.
 * - a separação clara entre `ICommandEncoder` (mutável) e `ICommandBuffer` (imutável) permite uma melhor organização do código e facilita a implementação de backends que podem otimizar a representação interna do `ICommandBuffer` finalizado, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.    
 * - a implementação de backend pode otimizar a representação interna do `ICommandBuffer` finalizado, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.    
 */
class ICommandBuffer {
public:
    // Destrutor virtual para permitir destruição polimórfica.
    virtual ~ICommandBuffer() = default;

    /**
     * @brief Descritor usado na finalização do buffer.
     * Decisão de design:
     * - inclui rótulo opcional para depuração/performance;
     * - inclui flag de one-time submit para otimizações de backend.
     * - o contrato do `CommandBufferDesc` permite que a implementação de backend otimize a criação e execução do `ICommandBuffer` finalizado, por exemplo, alocando recursos de forma mais eficiente para buffers que serão submetidos apenas uma vez, ou incluindo o rótulo em ferramentas de depuração e análise de desempenho, sem quebrar a compatibilidade com a API pública.
     */
    [[nodiscard]] virtual const CommandBufferDesc& desc() const noexcept = 0;

    /**
     * @brief Quantidade total de comandos gravados.
     * Decisão de design:
     * - expõe contagem total para validação e análise de desempenho;
     * - pode ser estendido no futuro para incluir contagens por tipo de comando ou outras métricas relevantes.
     * - a contagem total de comandos gravados pode ser usada para validar a lógica de gravação e finalização de comandos, bem como para análise de desempenho, sem depender de implementações específicas de backends gráficos. A implementação de backend pode otimizar a forma como essa contagem é mantida e exposta, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.    
     */
    [[nodiscard]] virtual std::uint64_t command_count() const noexcept = 0;
};

}  // namespace vme::engine::gfx::commands
