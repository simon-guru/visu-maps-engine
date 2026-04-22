/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <vector>

#include "engine/gfx/commands/i_command_buffer.hpp"

/**
 * @brief Contrato de fila gráfica para submissão e processamento.
 * Decisões suportadas:
 * - expõe apenas operações essenciais para manter foco e simplicidade;
 * - inclui mecanismos de sincronização para permitir coordenação entre submissões;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - a fila gráfica é projetada para ser um contrato mínimo que pode ser implementado por diferentes backends gráficos, permitindo flexibilidade e abstração na criação de recursos gráficos.
 * - a fila gráfica inclui mecanismos de sincronização, como timelines e fences, para permitir coordenação entre submissões e garantir a correta ordem de execução dos comandos, sem depender de implementações específicas de backends gráficos.
 * - a fila gráfica evita propagar códigos de erro específicos de backend para a API pública, simplificando o contrato e permitindo que cada backend implemente suas próprias estratégias de tratamento de erros, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.
 */
namespace vme::engine::gfx::contracts {

/**
 * @brief Valor monotônico de timeline para sincronização incremental.
 * Decisão de design:
 * - struct simples para encapsular valor de timeline;
 * - permite evolução futura para incluir múltiplas timelines ou outros mecanismos de sincronização.
 * - a estrutura `QueueTimeline` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas timelines, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `QueueTimeline` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct QueueTimeline {
    // Valor monotônico de timeline, incrementado a cada submissão.
    std::uint64_t value{0};
};

/**
 * @brief Valor explícito de fence usado para sinalização por submissão.
 * Decisão de design:
 * - struct simples para encapsular valor de fence;
 * - permite evolução futura para incluir múltiplas fences ou outros mecanismos de sincronização.
 * - a estrutura `FenceValue` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas fences, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `FenceValue` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct FenceValue {
    // Valor explícito de fence, usado para sinalização por submissão.
    std::uint64_t value{0};
};

/**
 * @brief Estado de fence com último valor concluído pela fila.
 * Decisão de design:
 * - struct simples para encapsular estado de fence;
 * - permite evolução futura para incluir múltiplas fences ou outros mecanismos de sincronização.
 * - a estrutura `FenceState` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas fences, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `FenceState` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct FenceState {
    // Último valor de fence concluído pela fila, atualizado após cada submissão processada.
    FenceValue completed_value{};
};

/**
 * @brief Dependência de espera em timeline.
 * Decisão de design:
 * - struct simples para encapsular referência a timeline e valor mínimo esperado;
 * - permite evolução futura para incluir múltiplas timelines ou outros mecanismos de sincronização.
 * - a estrutura `TimelineWaitInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas timelines, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `TimelineWaitInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct TimelineWaitInfo {
    // Referência à timeline a ser aguardada, deve ser válida durante a submissão.
    QueueTimeline* timeline{nullptr};
    // Valor mínimo esperado da timeline para que a submissão possa ser processada.
    std::uint64_t min_value{1};
};

/**
 * @brief Sinalização de timeline após execução da submissão.
 * Decisão de design:
 * - struct simples para encapsular referência a timeline e valor a ser sinalizado;
 * - permite evolução futura para incluir múltiplas timelines ou outros mecanismos de sincronização.
 * - a estrutura `TimelineSignalInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas timelines, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `TimelineSignalInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct TimelineSignalInfo {
    // Referência à timeline a ser sinalizada, deve ser válida durante a submissão.
    QueueTimeline* timeline{nullptr};
    // Valor a ser sinalizado na timeline após a submissão ser processada.
    std::uint64_t value{1};
};  

/**
 * @brief Sinalização de fence após execução da submissão.
 * Decisão de design:
 * - struct simples para encapsular referência a fence e valor a ser sinalizado;
 * - permite evolução futura para incluir múltiplas fences ou outros mecanismos de sincronização.
 * - a estrutura `FenceSignalInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas fences, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `FenceSignalInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
struct FenceSignalInfo {
    // Referência à fence a ser sinalizada, deve ser válida durante a submissão.
    FenceState* fence{nullptr};
    // Valor a ser sinalizado na fence após a submissão ser processada.
    FenceValue value{1};
};

/**
 * @brief Informações de uma única submissão para fila.
 * Decisão de design:
 * - struct simples para encapsular comandos e sincronização;
 * - permite evolução futura para incluir múltiplos command buffers ou outros mecanismos de sincronização.
 * - a estrutura `SubmitInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplos command buffers, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `SubmitInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a submissão e sincronização conforme necessário.
 */
struct SubmitInfo {
    //  Ponteiro para command buffer a ser submetido, deve ser válido durante a submissão.
    const commands::ICommandBuffer* command_buffer{nullptr};
    // Listas de dependências de espera e sinalização para coordenação entre submissões.
    std::vector<TimelineWaitInfo> waits{};
    // Listas de sinalizações para atualizar timelines e fences após a submissão ser processada.
    std::vector<TimelineSignalInfo> signals{};
    // Sinalização de fence para coordenação com sistemas externos ou outras filas, opcional.
    FenceSignalInfo fence_signal{};
};

/**
 * @brief Lote de submissões processadas em ordem estável de inserção.
 * Decisão de design:
 * - struct simples para encapsular múltiplas submissões;
 * - permite evolução futura para incluir mecanismos adicionais de coordenação ou agrupamento.
 * - a estrutura `SubmitBatch` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como mecanismos de coordenação, agrupamento, etc.) sem quebrar o contrato existente.
 * - a estrutura `SubmitBatch` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre o lote de submissões conforme necessário.
 */
struct SubmitBatch {
    // Lista de submissões a serem processadas em ordem estável de inserção.
    std::vector<SubmitInfo> submissions{};
};

/**
 * @brief Contrato de fila gráfica para submissão e processamento.
 * Decisões suportadas:
 * - expõe apenas operações essenciais para manter foco e simplicidade;
 * - inclui mecanismos de sincronização para permitir coordenação entre submissões;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - a fila gráfica é projetada para ser um contrato mínimo que pode ser implementado por diferentes backends gráficos, permitindo flexibilidade e abstração na criação de recursos gráficos.
 * - a fila gráfica inclui mecanismos de sincronização, como timelines e fences, para permitir coordenação entre submissões e garantir a correta ordem de execução dos comandos, sem depender de implementações específicas de backends gráficos.
 * - a fila gráfica evita propagar códigos de erro específicos de backend para a API pública, simplificando o contrato e permitindo que cada backend implemente suas próprias estratégias de tratamento de erros, desde que o contrato público seja mantido, permitindo melhor desempenho sem sacrificar a correção.
 */
enum class QueueSubmitErrorCode : std::uint16_t {
    Ok = 0, // Submissão processada com sucesso.
    InvalidArgument, // Ex: submissão com command buffer nulo ou inválido.
    SyncUnresolved, // Ex: submissão com dependência de timeline não atendida.
    InternalFailure, // Ex: erro interno do backend durante processamento da submissão.
};

/**
 * @brief Resultado de submissão/processamento de fila.
 * Decisão de design:
 * - struct simples para encapsular código de erro e mensagem opcional;
 * - método `ok()` para facilitar verificações de sucesso;
 * - método estático `ok_result()` para criar facilmente um resultado de sucesso.
 * - evita o uso de exceções para erros, simplificando o fluxo de controle.
 * - a estrutura `QueueSubmitResult` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos, mensagens, etc.) sem quebrar o contrato existente.
 * - a estrutura `QueueSubmitResult` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
struct QueueSubmitResult {
    // Código de erro, padrão é Ok para indicar sucesso.
    QueueSubmitErrorCode code{QueueSubmitErrorCode::Ok};
    // Mensagem de erro opcional para detalhes adicionais.
    const char* message{nullptr};

    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok(void) 
     const noexcept { 
        return code == QueueSubmitErrorCode::Ok; 
    }

    // Método estático para criar um resultado de sucesso.
    inline static QueueSubmitResult ok_result(void) 
    noexcept { 
        return {}; 
    }
};

/**
 * @brief Contrato de fila gráfica para submissão e processamento.
 */
class IGfxQueue {
public:
    virtual ~IGfxQueue() = default;

    /**
     * @brief Agenda uma submissão para execução assíncrona na fila.
     */
    virtual QueueSubmitResult submit(const SubmitInfo& submit_info) = 0;

    /**
     * @brief Agenda múltiplas submissões respeitando ordem estável do lote.
     */
    virtual QueueSubmitResult submit_batch(const SubmitBatch& submit_batch) = 0;

    /**
     * @brief Processa próxima submissão pendente e aplica sinalizações.
     */
    virtual QueueSubmitResult process_next_submission() = 0;

    /**
     * @brief Quantidade de submissões pendentes de execução.
     */
    [[nodiscard]] virtual std::uint64_t pending_submission_count() const noexcept = 0;

    /**
     * @brief Quantidade de submissões já processadas pela fila.
     */
    [[nodiscard]] virtual std::uint64_t completed_submission_count() const noexcept = 0;
};

}  // namespace vme::engine::gfx::contracts
