# engine/core/src

Implementação interna do `engine/core`.

## Propósito

Conter implementações concretas dos contratos públicos expostos em `engine/core/include`.

## Implementações registradas

- `types_engine_models.cpp`
  - implementação de `to_string(EngineState)`;
  - implementação de `to_string(EngineErrorSeverity)`.
- `lifecycle/engine_lifecycle_controller.cpp`
  - implementação concreta do lifecycle (`initialize`, `tick`, `shutdown`, `state`).

## Regras desta fase

- preservar separação clara entre interface (include) e implementação (src);
- evitar dependências diretas de backend gráfico;
- manter cada implementação próxima do seu contrato correspondente.
