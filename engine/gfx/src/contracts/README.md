# engine/gfx/src/contracts

Implementações base para contratos (`Instance`, `Device`, `Queue`, `Swapchain`).

## Escopo de implementação

- `StubGfxQueue` com fila interna, `SubmitInfo`/`SubmitBatch`, monotonicidade de timeline/fence e sinalização básica;
- `StubGfxSwapchain` com ciclo mínimo acquire/present/recreate;
- `StubGfxDevice` com criação de fila e swapchain;
- `StubGfxInstance` com seleção de adapter e criação de device para fluxo end-to-end.
