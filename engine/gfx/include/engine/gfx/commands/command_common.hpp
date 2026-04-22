/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>

namespace vme::engine::gfx::commands {

/**
 * @brief Código de erro da camada de comandos.
 *
 * Decisão de design:
 * - enum fechado para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 */
enum class CommandErrorCode : std::uint16_t {
    Ok = 0, // Valor explícito para facilitar verificações de sucesso.
    InvalidState, // Ex: gravar comando após finish.
    InvalidArgument, // Ex: draw com vertex_count=0.
    CapabilityNotSupported, // Ex: usar recurso não suportado pelo backend.
    OutOfMemory, // Ex: falha ao alocar memória para comando.
    BackendFailure, // Ex: erro interno do backend durante gravação/finalização.
};

/**
 * @brief Resultado padrão para operações de gravação/finalização de comandos.
 * Decisão de design:
 * - struct simples para encapsular código de erro e mensagem opcional;
 * - método `ok()` para facilitar verificações de sucesso;
 * - método estático `ok_result()` para criar facilmente um resultado de sucesso.
 * - evita o uso de exceções para erros, simplificando o fluxo de controle.
 */
struct CommandError {
    // Código de erro, padrão é Ok para indicar sucesso.
    CommandErrorCode code{CommandErrorCode::Ok};
    // Mensagem de erro opcional para detalhes adicionais.
    std::string message{};

    //  Método auxiliar para verificar se o resultado indica sucesso.
    [[nodiscard]] inline bool ok(void) 
    const noexcept { 
        return code == CommandErrorCode::Ok; 
    }

    // Método estático para criar um resultado de sucesso.
    inline static CommandError ok_result(void) 
    noexcept { 
        return {}; 
    }
};

/**
 * @brief Metadados declarativos de um command buffer.
 * Decisão de design:
 * - inclui um rótulo opcional para depuração e análise de desempenho;
 * - inclui uma flag de one_time_submit para otimizações de backend;
 * - pode ser estendido no futuro com campos adicionais sem quebrar o contrato.
 */
struct CommandBufferDesc {
    // Rótulo opcional para identificação do command buffer em ferramentas de depuração/performance.
    std::string label{};
    // Flag que indica se o command buffer será submetido apenas uma vez, permitindo otimizações de backend.
    bool one_time_submit{false};
};

/**
 * @brief Operação de cópia linear entre buffers.
 * Decisão de design:
 * - inclui offsets e tamanho para permitir cópias parciais;
 * - validações de argumentos garantem que a operação seja consistente e segura.
 * - pode ser estendido no futuro para incluir opções adicionais (ex: cópia entre recursos
 */
struct CopyBufferCommand {
    // Offsets de origem e destino para a cópia, permitindo operações parciais.
    std::uint64_t src_offset{0};
    // Offset de destino para a cópia, permitindo operações parciais.
    std::uint64_t dst_offset{0};
    // Tamanho da cópia em bytes, deve ser maior que zero para ser válida.
    std::uint64_t size_bytes{0};
};

/**
 * @brief Operação de dispatch para workloads de compute.
 * Decisão de design:
 * - inclui contagem de grupos para cada dimensão, permitindo dispatchs 1D, 2D e 3D;
 * - validações de argumentos garantem que a operação seja consistente e segura.
 * - pode ser estendido no futuro para incluir opções adicionais (ex: indirect dispatch).
 */
struct DispatchCommand {
    // Contagem de grupos de trabalho para cada dimensão, deve ser maior que zero para ser válida.
    std::uint32_t group_count_x{1};
    // Contagem de grupos de trabalho para a dimensão Y, deve ser maior que zero para ser válida.
    std::uint32_t group_count_y{1};
    // Contagem de grupos de trabalho para a dimensão Z, deve ser maior que zero para ser válida.
    std::uint32_t group_count_z{1};
};

/**
 * @brief Operação de draw para workloads gráficos.
 * Decisão de design:
 * - inclui contagens de vértices e instâncias, permitindo draw calls flexíveis;
 * - inclui offsets para draw indireto e instanciamento, permitindo operações avançadas;
 * - validações de argumentos garantem que a operação seja consistente e segura.
 * - pode ser estendido no futuro para incluir opções adicionais (ex: draw indexed, indirect draw). 
 */
struct DrawCommand {
    // Contagem de vértices a serem desenhados, deve ser maior que zero para ser válida.
    std::uint32_t vertex_count{0};
    // Contagem de instâncias a serem desenhadas, deve ser maior que zero para ser válida.
    std::uint32_t instance_count{1};
    // Offset do primeiro vértice para draw indireto, permitindo operações avançadas.
    std::uint32_t first_vertex{0};
    // Offset da primeira instância para draw indireto, permitindo operações avançadas.
    std::uint32_t first_instance{0};
};

/**
 * @brief Escopo de estágios abstratos para sincronização.
 * Decisão de design:
 * - enum fechado para manter contrato estável entre implementações;
 * - representa estágios comuns de pipeline gráfico e de computação;
 * - pode ser estendido no futuro para incluir estágios adicionais conforme necessário.
 */
enum class PipelineStage : std::uint8_t {
    Top, // Estágio inicial, antes de qualquer outro estágio.
    Transfer, // Estágio de transferência de dados (ex: cópias de buffer/texture).
    Vertex, // Estágio de vértices.
    Fragment, // Estágio de fragmentos.
    Compute, // Estágio de computação.
    Bottom, // Estágio final, depois de qualquer outro estágio.
};

/**
 * @brief Operação de barreira abstrata entre estágios.
 * Decisão de design:
 * - inclui estágios de origem e destino para sincronização flexível;
 * - validações de argumentos garantem que a operação seja consistente e segura.
 * - pode ser estendido no futuro para incluir opções adicionais (ex: dependências de recursos, máscaras de acesso).    
 * - a implementação de backend pode traduzir essa barreira abstrata para as primitivas de sincronização específicas da API gráfica subjacente, mantendo a portabilidade e simplicidade da API pública. 
 * - a implementação de backend pode otimizar ou combinar barreiras quando possível, desde que o contrato de sincronização seja mantido, permitindo melhor desempenho sem sacrificar a correção.    
 */
struct BarrierCommand {
    // Estágio de origem para a barreira, indicando o estágio do pipeline onde a sincronização começa.  
    PipelineStage src_stage{PipelineStage::Top};
    // Estágio de destino para a barreira, indicando o estágio do pipeline onde a sincronização termina.    
    PipelineStage dst_stage{PipelineStage::Bottom};
};

}  // namespace vme::engine::gfx::commands
