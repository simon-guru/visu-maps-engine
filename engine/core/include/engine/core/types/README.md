# engine/core/include/engine/core/types

Tipos fundamentais do engine.

## Escopo

Nesta fase, este diretório deverá evoluir para conter:

- identificadores e handles (`Id`, `Handle`, `LayerId`);
- tipos de tempo e frame (`Timestamp`, `DeltaTime`, `FrameIndex`);
- tipos geométricos básicos (`Vec2`, `Vec3`, `Rect`, `Bounds`);
- enums de estado mínimos (ex.: `LifecycleState`).

## Diretrizes

- sem dependências de backend;
- foco em tipos triviais/copáveis quando possível;
- sem regras de negócio de módulos específicos.
