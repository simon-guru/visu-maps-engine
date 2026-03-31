# Funcionalidades do VISU Maps Engine

Este documento consolida o que já está **conceituado**, o que já tem **implementação real** e o que precisa de **reavaliação** antes da fase de código pesado.

Legenda de status:
- `CONCEITUAL`: visão definida, sem código consolidado.
- `EM ANDAMENTO`: implementação iniciada na branch.
- `IMPLEMENTADO`: funcionalidade operante e validada.
- `REAVALIAR`: precisa de decisão/ADR antes de codar.

## Matriz de funcionalidades

| Área | Funcionalidade | Status | Observações |
|---|---|---|---|
| Render | Pipeline base de desenho de mapa | CONCEITUAL | Contratos devem nascer em `engine/render` + `engine/gfx`. |
| Render | Composição por camadas (base/labels/overlays) | CONCEITUAL | Ordenação e política de blending ainda não fixadas. |
| Render | Marcadores customizáveis (ícone/cor/tamanho/label) | CONCEITUAL | Inclui regiões, pessoas, objetos e pontos de interesse. |
| Render | Atualização de marcadores em tempo real | REAVALIAR | Definir budget de atualização por frame e estratégia de diff. |
| Tiles | Carregamento de tiles vetoriais/raster | CONCEITUAL | Formatos e contrato de normalização pendentes. |
| Tiles | Cache em memória/disco | REAVALIAR | Definir estratégia de invalidação e orçamento de cache. |
| Serviços | Geocodificação e busca | CONCEITUAL | Definir provider abstraction e fallback. |
| Serviços | Roteamento | CONCEITUAL | Integrar com telemetria de navegação depois de MVP visual. |
| Serviços | Stream de eventos para marcadores (add/update/remove) | CONCEITUAL | API de eventos para sincronismo near-real-time. |
| Sensores | GNSS + heading | CONCEITUAL | Estratégia de qualidade do sinal precisa de RFC/ADR. |
| Plataformas | Host Android/iOS/Web/Desktop | CONCEITUAL | Estruturas de apps prontas para integração inicial. |
| Backends | Vulkan/Metal/GLES/WebGL/GL46 | CONCEITUAL | Priorizar 1 backend de referência no primeiro milestone. |
| Shaders | Pipeline GLSL -> SPIR-V -> MSL | CONCEITUAL | Fechar toolchain e validação automatizada. |
| Testes | Unitários e integração | CONCEITUAL | Definir baseline mínimo por módulo antes de feature work. |
| Tooling | Asset pipeline e debug tools | CONCEITUAL | Entram com os primeiros assets reais e capturas de frame. |

## Marcadores em tempo real (escopo funcional)

### Entidades suportadas

- **Regiões**: polígonos/áreas com estilo customizável.
- **Pessoas**: marcador com posição dinâmica e metadados.
- **Objetos**: ícones/símbolos customizados com rotação/estado.
- **POIs/outros**: categorias abertas com estilo por tema.

### Operações mínimas da API

- `addMarker` / `addRegion`
- `updateMarker` / `updateRegion`
- `removeMarker` / `removeRegion`
- `setMarkerStyle` / `setLayerStyle`
- `subscribeMarkerEvents` (entrada de stream)

## Reavaliações recomendadas (antes de codar)

1. **Backend de referência do MVP**: Vulkan (desktop/dev) ou GLES (mais amplo mobile).
2. **Modelo de tiles inicial**: começar com raster para reduzir risco ou iniciar direto em vector tile.
3. **Contratos da API interna**: congelar interfaces mínimas em `engine/core` e `engine/render`.
4. **Política de cache**: estabelecer limites por dispositivo/plataforma.
5. **Observabilidade**: padronizar logs/métricas desde o primeiro frame útil.
6. **Tempo real de marcadores**: definir SLA de latência (ex.: < 250ms ponta-a-ponta) e política de coalescência de eventos.

## Critério de “feature pronta” (proposta)

Uma feature só deve migrar para `IMPLEMENTADO` quando tiver:
- contrato documentado;
- implementação mínima funcional em pelo menos um backend/plataforma;
- teste automatizado (unitário ou integração);
- evidência de execução (log/benchmark/screenshot quando aplicável).
