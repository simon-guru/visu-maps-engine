# engine/core/include/engine/core/types

Tipos fundamentais e contratos estáveis do engine.

## Escopo

Nesta fase, este diretório contém os modelos-base do core:

- `EngineState`: enum de estado global (`Uninitialized`, `Initialized`, `Running`, `Paused`, `Stopping`, `Stopped`);
- `EngineErrorSeverity`: classificação semântica de erro (`Info`, `Warning`, `Recoverable`, `Fatal`);
- `EngineError`: modelo padrão de erro com `code`, `severity` e `message`;
- `EngineConfig`: configuração mínima de bootstrap (`app_name`, `target_fps`, validações);
- `FrameContext`: metadados por frame (`frame_index`, `delta_time`, `timestamp`);
- `to_string(...)`: helpers para serializar enums em logs e diagnósticos;
- `lifecycle_error::*`: catálogo de códigos/mensagens estáveis do lifecycle.

## Declarações públicas

As declarações estão em:

- `engine/core/types/engine_models.hpp`
- `engine/core/types/engine_error_codes.hpp`

## Catálogo de erros do lifecycle (estável para host)

> **Contrato de integração:** os pares (`code`, `message`) abaixo devem permanecer
> estáveis para evitar quebra de parsing/telemetria no host.

| Identificador | Código | Severidade | Condição de disparo | Mensagem estável |
|---|---:|---|---|---|
| `LifecycleInitializeInvalidState` | `1001` | `Recoverable` | `initialize()` chamado fora de `Uninitialized`. | `initialize requer estado Uninitialized.` |
| `LifecycleInvalidConfig` | `1002` | `Recoverable` | `EngineConfig` inválido (`app_name` vazio ou `target_fps <= 0`). | `EngineConfig inválido: app_name vazio ou target_fps <= 0.` |
| `LifecycleTickInvalidState` | `1003` | `Recoverable` | `tick()` chamado fora de `Running`/`Initialized` (após promoção). | `tick requer estado Running ou Initialized.` |
| `LifecycleInvalidFrameContext` | `1004` | `Recoverable` | `FrameContext.delta_time` negativo. | `FrameContext inválido: delta_time negativo.` |
| `LifecycleShutdownInvalidState` | `1005` | `Recoverable` | `shutdown()` chamado em `Uninitialized`. | `shutdown requer engine inicializada.` |
| `LifecyclePauseInvalidState` | `1006` | `Recoverable` | `pause()` chamado fora de `Running`. | `pause requer estado Running.` |
| `LifecycleResumeInvalidState` | `1007` | `Recoverable` | `resume()` chamado fora de `Paused`. | `resume requer estado Paused.` |

## Comentários detalhados no código

Os headers foram documentados com comentários precisos explicando:

- a responsabilidade de cada tipo;
- o motivo de uso de `enum class` (tipagem forte e segurança);
- o papel operacional de cada campo de configuração e frame;
- a finalidade diagnóstica dos helpers `to_string(...)`;
- o racional do catálogo estável de erros para integração host.

## Diretrizes

- sem dependências de backend;
- foco em tipos triviais/copáveis quando possível;
- sem regras de negócio de módulos específicos;
- mudanças em códigos/mensagens de erro exigem revisão de compatibilidade com host.
