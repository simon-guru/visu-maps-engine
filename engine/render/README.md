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
- `SceneRenderData` é construído a partir de snapshots do cache de tiles (`tiles::RenderSnapshotCache`).
- cache de descritores de material/pipeline por cena com invalidação por eventos de tema/estilo (`services::ThemeStyleChangedEvent`) e revisões de pipeline.

## Próximo foco

- conectar `RenderSnapshotCache` ao pipeline real de ingestão de tiles;
- integrar invalidação de cache com o bus de eventos de `services`;
- introduzir validações de compatibilidade material/pass por tipo de pipeline.
