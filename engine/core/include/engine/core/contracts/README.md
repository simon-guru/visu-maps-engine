# engine/core/include/engine/core/contracts

Interfaces comuns compartilhadas pelos módulos do engine.

## Escopo

Este diretório concentra contratos transversais, por exemplo:

- módulos inicializáveis/encerráveis;
- serviços atualizáveis por frame;
- componentes observáveis por eventos do core.

## Declarações públicas desta etapa

- `engine/core/contracts/i_engine_lifecycle.hpp`
  - interface `IEngineLifecycle` com operações:
    - `initialize(const EngineConfig&)`;
    - `tick(const FrameContext&)`;
    - `pause()`;
    - `resume()`;
    - `shutdown()`;
    - `state() const`;
    - `config() const`.
- `engine/core/i_engine_lifecycle.hpp`
  - facade de compatibilidade de include path para consumidores externos.

## Regras normativas do contrato `IEngineLifecycle`

As regras abaixo **espelham exatamente** as seções dos comentários do header.

### `initialize(const EngineConfig&)`
- Pré-condições: entrada permitida somente em `EngineState::kUninitialized`.
- Pós-condições de sucesso: finaliza em `EngineState::kRunning`; `config()` passa a refletir a configuração efetiva; runtime apto a `tick()`, `pause()` e `shutdown()`.
- Pós-condições de erro: preserva `EngineState::kUninitialized`.
- Thread-safety/reentrância: não thread-safe para concorrência com métodos mutáveis; não reentrante.

### `tick(const FrameContext&)`
- Pré-condições: entrada permitida somente em `EngineState::kRunning`.
- Pós-condições de sucesso: permanece em `EngineState::kRunning`; `config()` permanece estável.
- Pós-condições de erro: preserva `EngineState::kRunning`.
- Thread-safety/reentrância: não thread-safe para concorrência com métodos mutáveis; não reentrante.

### `pause()`
- Pré-condições: entrada permitida somente em `EngineState::kRunning`.
- Pós-condições de sucesso: finaliza em `EngineState::kPaused`; recursos críticos permanecem alocados para `resume()`.
- Pós-condições de erro: preserva `EngineState::kRunning`.
- Thread-safety/reentrância: não thread-safe para concorrência com métodos mutáveis; não reentrante.

### `resume()`
- Pré-condições: entrada permitida somente em `EngineState::kPaused`.
- Pós-condições de sucesso: finaliza em `EngineState::kRunning`; `config()` não é alterada.
- Pós-condições de erro: preserva `EngineState::kPaused`.
- Thread-safety/reentrância: não thread-safe para concorrência com métodos mutáveis; não reentrante.

### `shutdown()`
- Pré-condições: entrada permitida em `EngineState::kRunning` ou `EngineState::kPaused`.
- Pós-condições de sucesso: finaliza em `EngineState::kStopped`; `tick()`, `pause()` e `resume()` deixam de ser válidos.
- Pós-condições de erro: estado pode não ser preservado; implementação deve sinalizar em `EngineError` se houve encerramento parcial ou transição intermediária.
- Thread-safety/reentrância: não thread-safe para concorrência com métodos mutáveis; não reentrante.

### `state() const noexcept`
- Pré-condições: pode ser chamado em qualquer estado.
- Pós-condições de sucesso: não altera estado; leitura observacional sem efeito colateral.
- Pós-condições de erro: não aplicável (`noexcept` sem canal explícito de erro).
- Thread-safety/reentrância: thread-safe para leitura concorrente; reentrante.

### `config() const noexcept`
- Pré-condições: pode ser chamado em qualquer estado após inicialização bem-sucedida.
- Pós-condições de sucesso: não altera estado; retorna referência para configuração efetiva vigente.
- Pós-condições de erro: não aplicável (`noexcept` sem canal explícito de erro).
- Thread-safety/reentrância: thread-safe para leitura concorrente; reentrante.

## Diretrizes

- contratos curtos e orientados a capacidades;
- sem dependência circular entre módulos;
- nomeação explícita e previsível para facilitar manutenção.
