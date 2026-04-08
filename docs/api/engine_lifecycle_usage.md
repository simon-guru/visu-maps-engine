# Guia de uso do lifecycle do engine core

Este guia mostra como consumir o contrato `IEngineLifecycle` no app host com foco em:

- fluxo nominal de execução;
- tratamento de erros por `EngineError`;
- políticas de resiliência (log, retry, fallback, fail-fast);
- referência rápida para matriz de transições e catálogo de códigos.

> Referências de API:
> - contrato: `engine/core/include/engine/core/contracts/i_engine_lifecycle.hpp`;
> - implementação padrão: `engine/core/include/engine/core/lifecycle/engine_lifecycle_controller.hpp` e `engine/core/src/lifecycle/engine_lifecycle_controller.cpp`.

## 0) Logging estruturado opcional

O core expõe contrato público de logger estruturado em:

- `engine/core/include/engine/core/contracts/i_engine_logger.hpp`;
- `engine/core/include/engine/core/types/engine_logging.hpp`;
- `engine/core/include/engine/core/contracts/i_engine_tracer.hpp`;
- `engine/core/include/engine/core/types/engine_trace_events.hpp`.

Exemplo de logger host:

```cpp
#include <iostream>
#include "engine/core/contracts/i_engine_logger.hpp"

class StdoutEngineLogger final : public vme::engine::core::contracts::IEngineLogger {
public:
    void log(const vme::engine::core::types::EngineLogPayload& payload) override {
        std::cout << "[" << payload.module << "] "
                  << payload.event << " severity="
                  << vme::engine::core::types::to_string(payload.severity)
                  << " message=" << payload.message << "\n";
    }
};
```

`EngineLifecycleController` funciona normalmente sem logger (`nullptr`): o logging vira no-op sem alterar erros ou transições.


## 0.1) Tracing de frame (opcional e desacoplado de logger)

O tracing de frame usa contrato dedicado (`IEngineTracer`) para evitar acoplamento com `IEngineLogger`.

### Como habilitar

1. Defina `config.enable_frame_trace = true` no `initialize`.
2. Injete uma implementação de `IEngineTracer` no controller (via construtor ou `set_tracer`).

```cpp
#include <iostream>
#include "engine/core/contracts/i_engine_tracer.hpp"

class StdoutFrameTracer final : public vme::engine::core::contracts::IEngineTracer {
public:
    void trace_frame(const vme::engine::core::types::FrameTraceEvent& event) override {
        std::cout << "frame=" << event.frame_index
                  << " delta_time_ns=" << event.delta_time
                  << " timestamp_ns=" << event.timestamp << "\n";
    }
};
```

### Campos emitidos em cada frame

- `frame_index`: índice incremental do frame (`uint64`);
- `delta_time`: delta do frame em nanossegundos (`int64`);
- `timestamp`: timestamp monotônico do frame em nanossegundos (`int64`, epoch do `steady_clock`).

Observação de performance: quando `enable_frame_trace == false`, o caminho de `tick` retorna cedo (sem normalização temporal nem chamada de tracer).

## 1) Exemplo nominal: `initialize -> tick -> pause -> resume -> shutdown`

```cpp
#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"

using vme::engine::core::lifecycle::EngineLifecycleController;
using vme::engine::core::types::EngineConfig;
using vme::engine::core::types::FrameContext;

int main() {
    StdoutEngineLogger logger {};
    EngineLifecycleController engine {&logger};

    // Também é possível injetar depois: engine.set_logger(&logger);

    // 1) initialize
    EngineConfig config {};
    config.app_name = "host-app";
    config.target_fps = 60U;

    auto init_result = engine.initialize(config);
    if (!init_result.is_ok()) {
        return 1;
    }

    // 2) tick
    FrameContext frame {};
    frame.frame_index = 1U;
    frame.delta_time = FrameContext::Clock::duration::zero();

    auto tick_result = engine.tick(frame);
    if (!tick_result.is_ok()) {
        return 1;
    }

    // 3) pause
    auto pause_result = engine.pause();
    if (!pause_result.is_ok()) {
        return 1;
    }

    // 4) resume
    auto resume_result = engine.resume();
    if (!resume_result.is_ok()) {
        return 1;
    }

    // 5) shutdown
    auto shutdown_result = engine.shutdown();
    if (!shutdown_result.is_ok()) {
        return 1;
    }

    return 0;
}
```

## 2) Exemplo de erro: transição inválida + validação de `EngineError`

Exemplo: chamar `resume()` em estado `Running` (sem estar `Paused`) gera transição inválida com `code = 1007`.

```cpp
#include <cassert>
#include "engine/core/lifecycle/engine_lifecycle_controller.hpp"
#include "engine/core/types/engine_error_codes.hpp"

using namespace vme::engine::core;

int main() {
    lifecycle::EngineLifecycleController engine {};

    types::EngineConfig config {};
    auto init_result = engine.initialize(config);
    assert(init_result.is_ok());

    // Primeiro tick válido: Initialized -> Running.
    auto tick_result = engine.tick(types::FrameContext{});
    assert(tick_result.is_ok());

    // Erro proposital: resume em Running.
    auto err = engine.resume();

    // Validação completa de EngineError:
    assert(err.code == types::lifecycle_error::LifecycleResumeInvalidState);
    assert(err.severity == types::EngineErrorSeverity::Recoverable);
    assert(err.message == types::lifecycle_error::LifecycleResumeInvalidStateMessage);
}
```

Checklist de validação no host:

- `code`: obrigatório para decisão programática (telemetria, retry policy, fallback policy);
- `severity`: orienta criticidade (`Recoverable` vs `Fatal`);
- `message`: diagnóstico humano em logs e troubleshooting.

## 3) Recomendações de tratamento no host app

### Log estruturado

Sempre registrar, no mínimo:

- `operation` (`initialize`, `tick`, `pause`, `resume`, `shutdown`);
- `engine_state_at_call`;
- `error.code`;
- `error.severity`;
- `error.message`;
- `correlation_id` da requisição/sessão.

### Retry

Aplicar retry **somente** para erros recuperáveis e quando houver ação corretiva possível.

- Exemplo positivo: `1002` (`EngineConfig` inválido) após corrigir configuração dinâmica.
- Exemplo negativo: `1007` (`resume` fora de `Paused`) sem corrigir estado antes.

### Fallback

Fallback recomendado para manter experiência degradada:

- desabilitar loop de frame e manter UI informando estado degradado;
- usar snapshot/cached state ao invés de processamento contínuo;
- oferecer ação explícita de reinit controlado (quando permitido pelo fluxo do app).

### Fail-fast

Aplicar fail-fast quando:

- erro `Fatal`;
- violação de contrato que compromete consistência da sessão;
- repetição de erro de transição sem progresso de estado (loop de erro).

## 4) Mapeamento para matriz de transições e catálogo de códigos

### Matriz de transições (baseline implementada)

Consulte a seção **"Matriz de Transições do Lifecycle (baseline atual)"** em:

- `engine/core/README.md`

Link direto no repositório: `engine/core/README.md#matriz-de-transições-do-lifecycle-baseline-atual`

### Catálogo de códigos de erro

Fonte canônica dos códigos e mensagens estáveis:

- `engine/core/include/engine/core/types/engine_error_codes.hpp`

Códigos atuais:

- `1001`: `LifecycleInitializeInvalidState`
- `1002`: `LifecycleInvalidConfig`
- `1003`: `LifecycleTickInvalidState`
- `1004`: `LifecycleInvalidFrameContext`
- `1005`: `LifecycleShutdownInvalidState`
- `1006`: `LifecyclePauseInvalidState`
- `1007`: `LifecycleResumeInvalidState`

## Referências cruzadas

- Visão geral do core: `engine/core/README.md`
- Índice de APIs: `docs/api/README.md`
- README raiz: `README.md`
