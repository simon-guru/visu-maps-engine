# engine/gfx/include/engine/gfx/contracts

Interfaces centrais do módulo `engine/gfx`.

## Contratos mínimos

- `IGfxInstance`: criação/destruição do contexto gráfico global;
- `IGfxDevice`: recursos, pipelines, capacidades e filas;
- `IGfxQueue`: submissão, espera e sinalização;
- `IGfxSwapchain`: acquire/present/recreate;
- `IResourceAllocator`: estratégia de alocação/subalocação.

## Regras de contrato

- cada interface deve ter semântica clara de ownership;
- métodos devem retornar erros categorizados e diagnósticos úteis;
- suporte opcional precisa de consulta em `DeviceCaps` antes do uso;
- comportamento em perda de dispositivo/swapchain precisa ser previsível.
