# engine/render

Pipeline de renderização do mapa.

## Objetivos

- composição de camadas;
- ordenação de draw calls;
- gerenciamento de passes e qualidade visual;
- renderização de marcadores (regiões, pessoas, objetos, POIs).

## Status atual

- caminho mínimo de frame implementado com API pública de `engine/gfx`.
- plano de frame separado em passes (`background`, `tiles`, `overlays`).
- `SceneRenderData` agora é construído a partir de `tiles::MapRenderSnapshot`.
- cache de descritores de material/pipeline por cena com invalidação por revisão de estilo/pipeline.

## Próximo foco

- substituir snapshots de teste por snapshots vindos do fluxo real do módulo `tiles`;
- integrar política de invalidação por tema com eventos do módulo `services`;
- introduzir validações de compatibilidade material/pass por tipo de pipeline.
