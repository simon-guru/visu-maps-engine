# engine/core/tests

Testes do módulo `engine/core`.

## Estratégia

- `unit/`: valida contratos, tipos e utilitários de forma isolada;
- `integration/`: valida cenários de lifecycle ponta a ponta (bootstrap, tick, pause/resume, shutdown).

## Objetivo da fase

Criar base mínima de testes para proteger a evolução da camada core sem regressões.

## Execução contínua (local/CI)

- configurar: `cmake -S . -B build -DBUILD_TESTING=ON`;
- compilar: `cmake --build build`;
- executar gate do core: `ctest --test-dir build --output-on-failure -R '^engine_core\\.'`.

## Baseline inicial de cobertura

- consultar: `engine/core/tests/coverage_baseline.md`.
