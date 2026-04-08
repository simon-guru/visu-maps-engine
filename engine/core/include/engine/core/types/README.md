# engine/core/include/engine/core/types

Tipos fundamentais do engine.

## Escopo

Nesta fase, este diretório contém os modelos-base do core:

- `EngineState`: enum de estado global (`Uninitialized`, `Initialized`, `Running`, `Paused`, `Stopping`, `Stopped`);
- `EngineErrorSeverity`: classificação semântica de erro (`Info`, `Warning`, `Recoverable`, `Fatal`);
- `EngineError`: modelo padrão de erro com `code`, `severity` e `message`;
- `EngineConfig`: configuração mínima de bootstrap (`app_name`, `target_fps`, validações);
- `FrameContext`: metadados por frame (`frame_index`, `delta_time`, `timestamp`);
- `to_string(...)`: helpers para serializar enums em logs e diagnósticos.

## Declarações públicas

As declarações estão em:

- `engine/core/types/engine_models.hpp`

## Comentários detalhados no código

O header foi documentado com comentários precisos explicando:

- a responsabilidade de cada tipo;
- o motivo de uso de `enum class` (tipagem forte e segurança);
- o papel operacional de cada campo de configuração e frame;
- a finalidade diagnóstica dos helpers `to_string(...)`.

## Diretrizes

- sem dependências de backend;
- foco em tipos triviais/copáveis quando possível;
- sem regras de negócio de módulos específicos.
