# engine/core/src/contracts

Implementações auxiliares e adapters relacionados aos contratos do core.

## Escopo

- adapters padrão para interfaces comuns;
- implementação default de pontos de extensão simples;
- validações internas de compatibilidade entre contratos.

## Status nesta etapa

- contratos principais estão declarados em `include/engine/core/contracts`;
- a implementação concreta foi centralizada em `src/lifecycle/engine_lifecycle_controller.cpp`, onde o contrato `IEngineLifecycle` é aplicado.
