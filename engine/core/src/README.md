# engine/core/src

Implementação interna do `engine/core`.

## Propósito

Conter implementações concretas dos contratos públicos expostos em `engine/core/include`.

## Regras desta fase

- preservar separação clara entre interface (include) e implementação (src);
- evitar dependências diretas de backend gráfico;
- manter cada implementação próxima do seu contrato correspondente.
