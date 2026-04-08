# engine/core/src

Implementação interna do `engine/core`.

## Propósito

Conter implementações concretas dos contratos públicos expostos em `engine/core/include`.

## Implementações registradas

- `types_engine_models.cpp`
  - implementação de `to_string(EngineState)`;
  - implementação de `to_string(EngineErrorSeverity)`.
- `lifecycle/engine_lifecycle_controller.cpp`
  - implementação concreta do lifecycle (`initialize`, `tick`, `pause`, `resume`, `shutdown`, `state`, `config`).

## Comentários detalhados no código

As fontes `.cpp` foram atualizadas com comentários de responsabilidade e motivação de abordagem para:

- serialização de estados/severidades;
- validação de invariantes temporais do frame;
- gestão de transições de lifecycle com previsibilidade operacional.

## Regras desta fase

- preservar separação clara entre interface (include) e implementação (src);
- evitar dependências diretas de backend gráfico;
- manter cada implementação próxima do seu contrato correspondente.
