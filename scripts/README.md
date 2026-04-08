# scripts

Scripts auxiliares de automação de build, manutenção e validação do repositório.

## Diretriz

Cada script deve trazer:
- descrição do objetivo;
- parâmetros aceitos;
- exemplo de uso.

## Scripts disponíveis

### `run_core_observability_tests.sh`

- **Objetivo:** compilar e executar, em sequência, os testes unitários e de integração de observabilidade do `engine/core`, além do microbenchmark de `tick()`.
- **Parâmetros:** `iterations` (opcional, default `200000`) para o benchmark.
- **Exemplo de uso:**

```bash
./scripts/run_core_observability_tests.sh
./scripts/run_core_observability_tests.sh 300000
```
