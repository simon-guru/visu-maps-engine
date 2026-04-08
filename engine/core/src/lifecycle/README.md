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
  - controla `pause/resume` com transições explícitas;
  - controla `shutdown` com transições seguras para `Stopped`;
  - padroniza erros de transição inválida com códigos dedicados.

## Comentários detalhados no código

A fonte foi documentada em detalhe para explicar:

- por que cada lock é aplicado no escopo atual;
- por que cada transição é permitida/rejeitada;
- o racional dos códigos de erro por operação;
- o comportamento idempotente em shutdown de estados já finais.

## Meta da fase

Disponibilizar um fluxo confiável e determinístico para iniciar, executar frames e encerrar a engine.
