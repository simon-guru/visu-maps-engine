# engine/render

Pipeline de renderização do mapa.

## Objetivos

- composição de camadas;
- ordenação de draw calls;
- gerenciamento de passes e qualidade visual;
- renderização de marcadores (regiões, pessoas, objetos, POIs).

## Requisitos funcionais de marcadores

- customização por tema (ícone, cor, tamanho, texto);
- atualização dinâmica sem rebuild completo da cena;
- suporte a add/update/remove em lote;
- política de prioridade visual (z-order/layer).

## Status

Desenho conceitual pronto; implementação concreta pendente.
