# engine/core/src/lifecycle

Implementação do fluxo de ciclo de vida da engine.

## Escopo

- máquina de estados de lifecycle;
- orquestração de bootstrap/tick/shutdown;
- validação de transições de estado e ordenação de callbacks.

## Meta da fase

Disponibilizar um fluxo confiável e determinístico para iniciar, executar frames e encerrar a engine.
