# VISU Maps Engine

Monorepo do **VISU Maps Engine**, um motor multiplataforma para renderização, navegação e serviços de mapas.

> **Status geral (31/03/2026):**
> - ✅ Estrutura de pastas definida.
> - ✅ Base de build com CMake inicializada.
> - ✅ READMEs consolidados em toda a árvore principal.
> - ⚠️ Implementações C++ reais ainda em fase de entrada (scaffolding).

## Objetivo do projeto

Entregar um engine modular para:
- renderização 2D/3D de mapas;
- pipeline de tiles vetoriais/raster;
- integração com sensores e serviços de localização;
- execução consistente em mobile, desktop e web.

## Funcionalidades (o que faltou explicitar)

A consolidação completa das funcionalidades conceituais + status de implementação está em:

- [`docs/FEATURES.md`](docs/FEATURES.md)

Esse arquivo concentra:
- matriz de funcionalidades por área;
- status (`CONCEITUAL`, `EM ANDAMENTO`, `IMPLEMENTADO`, `REAVALIAR`);
- reavaliações técnicas antes da fase de código real;
- critério objetivo para marcar feature como pronta.

## Arquitetura (visão rápida)

- `engine/`: núcleo do motor e seus subsistemas.
- `backends/`: implementações gráficas por API (Vulkan, Metal, GLES, WebGL, etc.).
- `apps/`: aplicações host por plataforma.
- `tools/`: tooling de build, debug e pipeline de assets.
- `shaders/`: código shader por linguagem/alvo.
- `tests/`: testes unitários, integração e performance.
- `docs/`: decisões arquiteturais, API e visão técnica.
- `third_party/`: dependências externas e patches.

## Build inicial

Pré-requisitos mínimos:
- CMake `>= 3.31`
- Compilador C++20

Exemplo:

```bash
cmake -S . -B build
cmake --build build -j
```

## Roadmap de curto prazo

1. Fechar contratos de interfaces em `engine/core` e `engine/render`.
2. Subir primeiro backend funcional (Vulkan ou GLES) com draw básico.
3. Integrar pipeline de shaders e assets no CI.
4. Consolidar testes de unidade para módulos críticos.

## Navegação

- [Documentação](docs/README.md)
- [Funcionalidades](docs/FEATURES.md)
- [Engine](engine/README.md)
- [Backends](backends/README.md)
- [Apps](apps/README.md)
- [Shaders](shaders/README.md)
- [Tools](tools/README.md)
- [Testes](tests/README.md)
- [Third-party](third_party/README.md)
