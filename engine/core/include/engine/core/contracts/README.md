# engine/core/include/engine/core/contracts

Interfaces comuns compartilhadas pelos módulos do engine.

## Escopo

Este diretório concentra contratos transversais, por exemplo:

- módulos inicializáveis/encerráveis;
- serviços atualizáveis por frame;
- componentes observáveis por eventos do core.

## Declarações públicas desta etapa

- `engine/core/contracts/i_engine_lifecycle.hpp`
  - interface `IEngineLifecycle` com operações:
    - `initialize(const EngineConfig&)`;
    - `tick(const FrameContext&)`;
    - `pause()`;
    - `resume()`;
    - `shutdown()`;
    - `state() const`;
    - `config() const`.
- `engine/core/i_engine_lifecycle.hpp`
  - facade de compatibilidade de include path para consumidores externos.

## Comentários detalhados no código

Os headers de contrato foram atualizados para explicar com precisão:

- a responsabilidade de cada método do lifecycle;
- as razões de design da interface (estabilidade, desacoplamento e inspeção diagnóstica);
- o motivo da facade de include (compatibilidade e encapsulamento da estrutura interna).

## Diretrizes

- contratos curtos e orientados a capacidades;
- sem dependência circular entre módulos;
- nomeação explícita e previsível para facilitar manutenção.
