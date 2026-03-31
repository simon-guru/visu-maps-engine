# engine/core/include/engine/core/contracts

Interfaces comuns compartilhadas pelos módulos do engine.

## Escopo

Este diretório concentra contratos transversais, por exemplo:

- módulos inicializáveis/encerráveis;
- serviços atualizáveis por frame;
- componentes observáveis por eventos do core.

## Diretrizes

- contratos curtos e orientados a capacidades;
- sem dependência circular entre módulos;
- nomeação explícita e previsível para facilitar manutenção.
