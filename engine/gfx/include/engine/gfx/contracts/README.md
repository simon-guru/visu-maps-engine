# engine/gfx/include/engine/gfx/contracts

Interfaces centrais do módulo `engine/gfx`.

## Contratos mínimos

- `IGfxInstance`: criação/destruição do contexto gráfico global;
- `IGfxDevice`: recursos, pipelines, capacidades e criação de filas;
- `IGfxQueue`: submissão de `ICommandBuffer` para execução;
- `IGfxSwapchain`: acquire/present/recreate;
- `IResourceAllocator`: estratégia de alocação/subalocação.

## Estado atual da implementação

- `i_gfx_queue.hpp` define `SubmitInfo` e `SubmitBatch` com dependências explícitas;
- `QueueTimeline` e `FenceValue` formalizam sincronização incremental (timeline/fence por valor);
- validação de monotonicidade em sinais impede regressão de timeline/fence;
- `i_gfx_device.hpp` introduz `IGfxDevice::create_queue()`;
- `factory.hpp` expõe `create_gfx_queue_stub()` e `create_gfx_device_stub()` para testes end-to-end.

## Regras de contrato

- cada interface deve ter semântica clara de ownership;
- métodos devem retornar erros categorizados e diagnósticos úteis;
- suporte opcional precisa de consulta em `DeviceCaps` antes do uso;
- comportamento em perda de dispositivo/swapchain precisa ser previsível.
