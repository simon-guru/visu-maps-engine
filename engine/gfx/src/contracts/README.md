# engine/gfx/src/contracts

Implementações base para contratos (`Instance`, `Device`, `Queue`, `Swapchain`).

## Escopo de implementação

- gestão de estado de ciclo de vida (init, active, lost, disposed);
- regras de erro e propagação de diagnósticos;
- pontos de extensão para adapters concretos;
- `StubGfxQueue` com fila interna, `SubmitInfo`/`SubmitBatch`, monotonicidade de timeline/fence e sinalização básica;
- `StubGfxDevice` com `create_queue()` para integração end-to-end do fluxo device -> queue -> submit.
