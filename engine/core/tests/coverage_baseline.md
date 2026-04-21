# Baseline inicial de cobertura (Fase 4)

Este baseline documenta a cobertura funcional inicial por módulo de responsabilidade do `engine/core`, usando as suítes registradas no CTest.

> Escopo: baseline qualitativo de cenários críticos (não percentual), para gate inicial de regressão.

| Módulo / Responsabilidade | Cobertura inicial | Evidência de teste |
|---|---|---|
| `types` (`EngineConfig`, `EngineError`, serialização `to_string`) | **Coberto** | `engine_core.unit.types_contracts` |
| `contracts` (semântica pública de retorno e validação) | **Coberto** | `engine_core.unit.types_contracts` |
| `lifecycle` nominal (`bootstrap -> tick -> pause/resume -> shutdown`) | **Coberto** | `engine_core.integration.lifecycle_flow` |
| `lifecycle` + observabilidade habilitada | **Coberto** | `engine_core.unit.observability`, `engine_core.integration.observability_enabled` |
| `lifecycle` + observabilidade desabilitada | **Coberto** | `engine_core.unit.observability`, `engine_core.integration.observability_disabled` |

## Uso no gate de qualidade

1. Configurar build com CTest:
   - `cmake -S . -B build -DBUILD_TESTING=ON`
2. Compilar:
   - `cmake --build build`
3. Executar gate do core:
   - `ctest --test-dir build --output-on-failure -R '^engine_core\\.'`

Se qualquer teste falhar, o merge deve ser bloqueado.
