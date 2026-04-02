# engine/core/src/lifecycle

Implementação do fluxo de ciclo de vida da engine.

## Escopo

- máquina de estados de lifecycle;
- orquestração de bootstrap/tick/shutdown;
- validação de transições de estado e ordenação de callbacks.

## Implementações desta etapa

- `engine_lifecycle_controller.cpp`
  - valida `EngineConfig` no bootstrap;
  - promove `Initialized -> Running` no primeiro `tick`;
  - valida `FrameContext`;
  - controla `shutdown` com transições seguras para `Stopped`;
  - padroniza erros de transição inválida com códigos dedicados.

## Meta da fase

Disponibilizar um fluxo confiável e determinístico para iniciar, executar frames e encerrar a engine.
