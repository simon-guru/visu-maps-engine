/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// Dependência dos modelos base de configuração, estado e erro.
// Motivo: manter o contrato de lifecycle autocontido e semanticamente explícito.
#include "engine/core/types/engine_models.hpp"

namespace vme::engine::core::contracts {

/**
 * @brief Contrato base para governar o ciclo de vida do runtime.
 *
 * Responsabilidade:
 * - padronizar a sequência operacional comum para qualquer host da engine;
 * - reduzir acoplamento entre módulos por meio de uma capacidade mínima e explícita;
 * - permitir múltiplas implementações concretas mantendo API estável.
 */
class IEngineLifecycle {
public:
    // Destrutor virtual para garantir destruição correta via ponteiro para interface.
    virtual ~IEngineLifecycle() = default;

    /**
     * @brief Inicializa runtime com configuração validável.
     * @param config Configuração de bootstrap.
     * @return EngineError com `code == 0` em caso de sucesso.
     *
     * Pré-condições:
     * - estado de entrada permitido: `EngineState::kUninitialized`.
     *
     * Pós-condições de sucesso:
     * - estado final: `EngineState::kInitialized`;
     * - `config()` passa a refletir a configuração efetiva aplicada no bootstrap;
     * - invariantes: runtime pronto para receber `tick()`, `pause()` e `shutdown()`.
     *
     * Pós-condições de erro:
     * - estado deve ser preservado em `EngineState::kUninitialized`.
     *
     * Garantias de thread-safety e reentrância:
     * - não thread-safe para chamadas concorrentes com outros métodos mutáveis;
     * - não reentrante.
     */
    virtual types::EngineError initialize(const types::EngineConfig& config) = 0;

    /**
     * @brief Política de idempotência de `initialize`.
     *
     * - chamadas repetidas após inicialização bem-sucedida são inválidas;
     * - comportamento obrigatório: erro determinístico de transição inválida.
     */

    /**
     * @brief Processa um frame/tick da engine.
     * @param frame_context Metadados temporais do frame atual.
     * @return EngineError com resultado da operação.
     *
     * Pré-condições:
     * - estados de entrada permitidos: `EngineState::kInitialized` e `EngineState::kRunning`.
     *
     * Pós-condições de sucesso:
     * - estado final: permanece em `EngineState::kRunning`;
     * - invariantes: configuração exposta por `config()` permanece estável.
     *
     * Pós-condições de erro:
     * - estado deve ser preservado no estado de entrada
     *   (`EngineState::kInitialized` ou `EngineState::kRunning`).
     *
     * Garantias de thread-safety e reentrância:
     * - não thread-safe para chamadas concorrentes com outros métodos mutáveis;
     * - não reentrante.
     */
    virtual types::EngineError tick(const types::FrameContext& frame_context) = 0;

    /**
     * @brief Política de idempotência/validade de `tick`.
     *
     * - em estados não permitidos, retorno obrigatório de erro determinístico
     *   de transição inválida.
     */

    /**
     * @brief Move runtime para estado de pausa sem desalocar recursos críticos.
     *
     * Pré-condições:
     * - estado de entrada permitido: `EngineState::kRunning`.
     *
     * Pós-condições de sucesso:
     * - estado final: `EngineState::kPaused`;
     * - invariantes: recursos críticos permanecem alocados para permitir `resume()`.
     *
     * Pós-condições de erro:
     * - estado deve ser preservado em `EngineState::kRunning`.
     *
     * Garantias de thread-safety e reentrância:
     * - não thread-safe para chamadas concorrentes com outros métodos mutáveis;
     * - não reentrante.
     */
    virtual types::EngineError pause() = 0;

    /**
     * @brief Política de idempotência de `pause`.
     *
     * - chamadas repetidas (incluindo em `EngineState::kPaused`) são inválidas;
     * - comportamento obrigatório: erro determinístico de transição inválida.
     */

    /**
     * @brief Retoma execução normal a partir de estado pausado.
     *
     * Pré-condições:
     * - estado de entrada permitido: `EngineState::kPaused`.
     *
     * Pós-condições de sucesso:
     * - estado final: `EngineState::kRunning`;
     * - invariantes: configuração exposta por `config()` não é alterada.
     *
     * Pós-condições de erro:
     * - estado deve ser preservado em `EngineState::kPaused`.
     *
     * Garantias de thread-safety e reentrância:
     * - não thread-safe para chamadas concorrentes com outros métodos mutáveis;
     * - não reentrante.
     */
    virtual types::EngineError resume() = 0;

    /**
     * @brief Política de idempotência de `resume`.
     *
     * - chamadas repetidas (incluindo em `EngineState::kRunning`) são inválidas;
     * - comportamento obrigatório: erro determinístico de transição inválida.
     */

    /**
     * @brief Encerra runtime e libera recursos.
     *
     * Pré-condições:
     * - estados de entrada permitidos: `EngineState::kRunning` e `EngineState::kPaused`.
     *
     * Pós-condições de sucesso:
     * - estado final: `EngineState::kStopped`;
     * - invariantes: chamadas subsequentes de `tick()`, `pause()` e `resume()` deixam de ser válidas.
     *
     * Pós-condições de erro:
     * - estado pode não ser preservado; implementação deve registrar via `EngineError`
     *   se houve encerramento parcial ou transição intermediária.
     *
     * Garantias de thread-safety e reentrância:
     * - não thread-safe para chamadas concorrentes com outros métodos mutáveis;
     * - não reentrante.
     */
    virtual types::EngineError shutdown() = 0;

    /**
     * @brief Política de idempotência de `shutdown`.
     *
     * - chamadas repetidas em `EngineState::kStopping` ou `EngineState::kStopped`
     *   devem ser tratadas como no-op idempotente com sucesso (`code == 0`).
     */

    /**
     * @brief Fornece o estado atual da máquina de lifecycle.
     *
     * Pré-condições:
     * - pode ser chamado em qualquer estado.
     *
     * Pós-condições de sucesso:
     * - estado final: inalterado;
     * - invariantes: leitura observacional sem efeitos colaterais.
     *
     * Pós-condições de erro:
     * - não aplicável: método `noexcept` sem canal explícito de erro.
     *
     * Garantias de thread-safety e reentrância:
     * - thread-safe para leitura concorrente;
     * - reentrante.
     */
    [[nodiscard]] virtual types::EngineState state() const noexcept = 0;

    /**
     * @brief Exibe a configuração efetiva carregada no bootstrap.
     *
     * Motivo: permitir inspeção operacional e diagnóstica sem duplicar estado
     * fora do controller.
     *
     * Pré-condições:
     * - pode ser chamado em qualquer estado após uma inicialização bem-sucedida.
     *
     * Pós-condições de sucesso:
     * - estado final: inalterado;
     * - invariantes: retorna referência para configuração efetiva vigente.
     *
     * Pós-condições de erro:
     * - não aplicável: método `noexcept` sem canal explícito de erro.
     *
     * Garantias de thread-safety e reentrância:
     * - thread-safe para leitura concorrente;
     * - reentrante.
     */
    [[nodiscard]] virtual const types::EngineConfig& config() const noexcept = 0;
};

}  // namespace vme::engine::core::contracts
