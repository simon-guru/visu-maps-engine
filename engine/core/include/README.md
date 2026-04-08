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


## Implementado nesta etapa

- `types/engine_models.hpp` com modelos base e helpers de serialização.
- `contracts/i_engine_lifecycle.hpp` com contrato mínimo de runtime.
- `lifecycle/engine_lifecycle_controller.hpp` com implementação padrão pública de lifecycle.

- facade de compatibilidade: `engine/core/i_engine_lifecycle.hpp` encaminhando para `contracts/i_engine_lifecycle.hpp`.


## Documentação de comentários no código

Nesta fase, os headers públicos foram enriquecidos com comentários detalhados descrevendo responsabilidades, decisões de design e razões das abordagens adotadas em cada bloco relevante da API.
