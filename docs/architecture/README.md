# docs/architecture

Documenta a arquitetura de alto nível do engine.

## Escopo

- camadas do sistema;
- contratos entre módulos;
- regras de dependência;
- estratégia de escalabilidade e portabilidade.

## Reavaliações recomendadas

- Revisar responsabilidades entre `engine/core`, `engine/render` e `backends` antes da implementação pesada.
- Validar se a separação de `services` e `platform_abstraction` cobre todos os casos de plataforma.
