# engine/gfx/include/engine/gfx/contracts

Interfaces centrais do módulo `engine/gfx`.

## Contratos mínimos

- `IGfxInstance`: enumeração de adapters e criação de `IGfxDevice`;
- `IGfxDevice`: criação de filas (`IGfxQueue`), swapchain (`IGfxSwapchain`) e recursos (`Buffer/Texture/Sampler/ShaderModule/ResourceView`);
- `IGfxQueue`: submissão de `ICommandBuffer` para execução;
- `IGfxSwapchain`: acquire/present/recreate;
- `IResourceAllocator`: estratégia de alocação/subalocação.

## Estado atual da implementação

- `i_gfx_queue.hpp` define `SubmitInfo` e `SubmitBatch` com dependências explícitas;
- `QueueTimeline` e `FenceValue` formalizam sincronização incremental (timeline/fence por valor);
- validação de monotonicidade em sinais impede regressão de timeline/fence;
- `i_gfx_instance.hpp` + `i_gfx_device.hpp` permitem ciclo completo `instance -> device -> queue/swapchain`;
- `factory.hpp` expõe `create_gfx_instance_stub()`, `create_gfx_device_stub()` e `create_gfx_queue_stub()`.

## Regras de contrato

- cada interface deve ter semântica clara de ownership;
- métodos devem retornar erros categorizados e diagnósticos úteis;
- suporte opcional precisa de consulta em `DeviceCaps` antes do uso;
- comportamento em perda de dispositivo/swapchain precisa ser previsível.
