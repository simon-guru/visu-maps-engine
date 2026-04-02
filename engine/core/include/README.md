# engine/core/include

API pública do `engine/core`.

## Propósito

Concentrar headers estáveis consumidos pelos demais módulos do engine e pelas aplicações host.

## Regras desta fase

- evitar detalhes de implementação;
- priorizar tipos de valor simples e contratos pequenos;
- manter compatibilidade entre plataformas e backends.

## Estrutura

- `engine/core/types`: tipos fundamentais e aliases compartilhados.
- `engine/core/lifecycle`: estados, eventos e contratos de ciclo de vida.
- `engine/core/contracts`: interfaces comuns entre módulos.
- `engine/core/utils`: utilitários públicos (erro, logging e configuração).
