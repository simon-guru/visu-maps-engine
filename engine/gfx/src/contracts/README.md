# engine/gfx/src/contracts

Implementações base para contratos (`Instance`, `Device`, `Queue`, `Swapchain`).

## Escopo de implementação

- gestão de estado de ciclo de vida (init, active, lost, disposed);
- regras de erro e propagação de diagnósticos;
- pontos de extensão para adapters concretos;
- stub de `IGfxQueue` com fila interna, `SubmitInfo`/`SubmitBatch` e sinalização básica.
