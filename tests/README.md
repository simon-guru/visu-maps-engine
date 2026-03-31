# tests

Estratégia de testes do projeto.

## Suites

- `unit/`: valida comportamento isolado de módulos.
- `integration/`: valida fluxo entre subsistemas.
- `performance/`: mede custo de render, tiles e serviços.

## Cobertura funcional esperada

- render básico sem regressão de frame;
- pipeline de tiles com cache consistente;
- estabilidade de integração entre engine + backend + app host.

## Estado atual

Estrutura preparada; casos de teste ainda serão implementados.
