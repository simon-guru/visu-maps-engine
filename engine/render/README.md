# engine/render

Pipeline de renderização do mapa.

## Objetivos

- composição de camadas;
- ordenação de draw calls;
- gerenciamento de passes e qualidade visual;
- renderização de marcadores (regiões, pessoas, objetos, POIs).

## Status atual

- caminho mínimo de frame implementado em `basic_frame_renderer` usando apenas contratos públicos de `engine/gfx`.
- fluxo implementado: `acquire -> encode -> submit -> present`.

## Próximo foco

- evoluir do frame mínimo para passes separados (background, tiles, overlays);
- introduzir descritores de material/estado vinculados a pipelines de `engine/gfx`;
- conectar composição real de camadas e sistema de marcadores.
