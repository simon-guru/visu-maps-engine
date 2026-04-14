# engine/gfx/include

Headers públicos do módulo `engine/gfx`.

## Objetivo

Expor uma superfície de API pequena, estável e backend-agnóstica para recursos,
pipeline, comandos, sincronização e contratos de execução.

## Regras de design da superfície pública

- tipos e enums devem descrever intenção de uso, não detalhes de API concreta;
- cada descritor deve ser validável sem depender de chamadas específicas de backend;
- contratos devem explicitar ownership, thread-safety e estados inválidos;
- opcionais devem estar sempre vinculados a `DeviceCaps`.

## Implementação esperada nesta pasta

Nesta fase, os headers começam como contratos mínimos e evoluem de forma incremental.
Toda adição de API deve documentar:

1. problema que resolve;
2. impacto em pelo menos dois backends;
3. comportamento de fallback quando indisponível.
