# engine/gfx/include/engine/gfx/contracts

Interfaces centrais do módulo `engine/gfx`.

## Contratos mínimos

- `IGfxInstance`: criação/destruição do contexto gráfico global;
- `IGfxDevice`: recursos, pipelines, capacidades e filas;
- `IGfxQueue`: submissão de `ICommandBuffer` para execução;
- `IGfxSwapchain`: acquire/present/recreate;
- `IResourceAllocator`: estratégia de alocação/subalocação.

## Estado atual da implementação

- `i_gfx_queue.hpp` define `SubmitInfo` e `SubmitBatch` para suportar dependências explícitas de sincronização;
- `FenceState` e `SemaphoreState` formalizam sinalização básica CPU/GPU e GPU/GPU no contrato;
- `factory.hpp` expõe `create_gfx_queue_stub()` para testes de integração e validação de fluxo.

## Regras de contrato

- cada interface deve ter semântica clara de ownership;
- métodos devem retornar erros categorizados e diagnósticos úteis;
- suporte opcional precisa de consulta em `DeviceCaps` antes do uso;
- comportamento em perda de dispositivo/swapchain precisa ser previsível.
