# engine/core/include/engine/core/lifecycle

Contratos e tipos públicos do ciclo de vida da engine.

## Escopo

- estados da engine (uninitialized, initialized, running, paused, stopping, stopped);
- eventos de lifecycle (bootstrap concluído, início/fim de frame, shutdown);
- contratos mínimos de controle (`initialize`, `tick`, `shutdown`).

## Declarações públicas desta etapa

- `engine/core/lifecycle/engine_lifecycle_controller.hpp`
  - classe `EngineLifecycleController`;
  - implementação padrão do contrato `IEngineLifecycle`;
  - transições de estado para bootstrap/tick/pause/resume/shutdown.

## Objetivo da fase

Definir uma semântica única para o fluxo de execução da engine, reduzindo acoplamento entre módulos.
