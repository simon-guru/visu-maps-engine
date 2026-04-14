# engine/gfx/include/engine/gfx/contracts

Interfaces centrais do módulo `engine/gfx`.

## Contratos mínimos

- `IGfxInstance`: criação/destruição do contexto gráfico global;
- `IGfxDevice`: recursos, pipelines, capacidades e filas;
- `IGfxQueue`: submissão de `ICommandBuffer` para execução;
- `IGfxSwapchain`: acquire/present/recreate;
- `IResourceAllocator`: estratégia de alocação/subalocação.

## Estado atual da implementação

- `i_gfx_queue.hpp` foi introduzido como contrato mínimo inicial para submissão.
- demais contratos permanecem documentados e serão implementados nas próximas etapas.

## Regras de contrato

- cada interface deve ter semântica clara de ownership;
- métodos devem retornar erros categorizados e diagnósticos úteis;
- suporte opcional precisa de consulta em `DeviceCaps` antes do uso;
- comportamento em perda de dispositivo/swapchain precisa ser previsível.
