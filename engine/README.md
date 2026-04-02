# engine

Núcleo do VISU Maps Engine.

## Submódulos

- `core/`: tipos base, ciclo de vida e contratos centrais.
- `gfx/`: abstrações gráficas e recursos de GPU.
- `render/`: pipeline de renderização.
- `tiles/`: ingestão/caching de tiles.
- `services/`: serviços de mapas, rede e domínio.
- `sensors/`: integração com sensores (GNSS/IMU/etc.).
- `platform_abstraction/`: ponte para OS e recursos nativos.

## Funcionalidades foco da engine

- ciclo de vida do runtime;
- contrato de frame render;
- carregamento/caching de tiles;
- sincronização entre dados de sensores e câmera de mapa;
- contratos internos para desacoplamento de backends.

## Status (02/04/2026)

- Estrutura modular definida.
- `engine/core` já possui primeira implementação real de modelos e lifecycle.
- Priorização detalhada em `docs/FEATURES.md`.
