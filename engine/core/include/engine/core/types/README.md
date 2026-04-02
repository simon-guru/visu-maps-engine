# engine/core/include/engine/core/types

Tipos fundamentais do engine.

## Escopo

Nesta fase, este diretório evoluiu para conter os modelos-base do core:

- `EngineState`: enum de estado global (`Uninitialized`, `Initialized`, `Running`, `Paused`, `Stopping`, `Stopped`);
- `EngineErrorSeverity`: classificação semântica de erro (`Info`, `Warning`, `Recoverable`, `Fatal`);
- `EngineError`: modelo padrão de erro com `code`, `severity` e `message`;
- `EngineConfig`: configuração mínima de bootstrap (`app_name`, `target_fps`, validações);
- `FrameContext`: metadados por frame (`frame_index`, `delta_time`, `timestamp`);
- `to_string(...)`: helpers para serializar enums em logs e diagnósticos.

## Declarações públicas

As declarações estão em:

- `engine/core/types/engine_models.hpp`

## Diretrizes

- sem dependências de backend;
- foco em tipos triviais/copáveis quando possível;
- sem regras de negócio de módulos específicos.
