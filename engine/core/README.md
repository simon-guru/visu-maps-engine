# engine/core

Camada base do `visu-maps-engine`, responsável por contratos comuns, tipos fundamentais e coordenação do lifecycle que será consumido por `render`, `gfx`, `tiles`, `services`, `sensors` e `platform_abstraction`.

## Glossário conceitual (termos deste módulo)

- **Observabilidade**: capacidade de inferir o estado interno do runtime a partir de sinais externos (logs, traces e eventos), sem acoplar o core a ferramentas específicas.
- **Telemetria**: dados técnicos emitidos pela engine para monitoramento/diagnóstico operacional.
- **Log estruturado**: registro com campos bem definidos (chave-valor), adequado para indexação e busca automatizada.
- **Trace**: sequência temporal de eventos de execução (ex.: por frame) usada para entender latência e comportamento ao longo do tempo.
- **Lifecycle**: ciclo de vida do runtime, incluindo inicialização, execução por frames, pausa, retomada e encerramento.
- **Sink/Exporter**: destino de exportação de sinais de observabilidade; no core, é um contrato de interface sem dependência de backend.
- **No-op**: operação intencionalmente vazia, usada como fallback seguro quando observabilidade está desabilitada.
- **Backend-agnóstico**: desenho que evita dependência direta de tecnologia específica (vendor, protocolo ou biblioteca externa).


## Guia de uso do lifecycle (host apps)

Para um walkthrough objetivo com fluxo nominal, cenários de erro e recomendações de tratamento no app host, consulte:

- `docs/api/engine_lifecycle_usage.md`

## Fase atual (Fase 0 — Fundação do Core)

### Objetivo

Entregar uma base mínima, estável e backend-agnóstica para permitir evolução incremental dos demais módulos sem acoplamento prematuro.

### Escopo implementado nesta fase

- modelos fundamentais: `EngineConfig`, `EngineState`, `EngineError`, `FrameContext`;
- contrato de lifecycle: `IEngineLifecycle`;
- contrato de logging estruturado: `IEngineLogger` + `EngineLogPayload`;
- contrato de exportação de observabilidade: `IObservabilitySink`;
- envelope canônico de exportação: `EngineObservabilityEnvelope` + `EngineObservabilitySignal`;
- implementação padrão de lifecycle: `EngineLifecycleController`;
- include de compatibilidade: `engine/core/i_engine_lifecycle.hpp`;
- target CMake: `engine_core`.

### Organização de diretórios (estado atual)

```text
engine/core/
├── README.md
├── CMakeLists.txt
├── include/
│   ├── README.md
│   └── engine/core/
│       ├── i_engine_lifecycle.hpp
│       ├── types/
│       │   ├── README.md
│       │   ├── engine_models.hpp
│       │   └── engine_logging.hpp
│       ├── lifecycle/
│       │   ├── README.md
│       │   └── engine_lifecycle_controller.hpp
│       ├── contracts/
│       │   ├── README.md
│       │   ├── i_engine_lifecycle.hpp
│       │   └── i_engine_logger.hpp
│       └── utils/README.md
├── src/
│   ├── README.md
│   ├── types_engine_models.cpp
│   ├── lifecycle/
│   │   ├── README.md
│   │   └── engine_lifecycle_controller.cpp
│   ├── contracts/README.md
│   └── utils/README.md
├── tests/
│   ├── README.md
│   ├── unit/README.md
│   └── integration/README.md
└── cmake/
    └── README.md
```

### Critério adicional de documentação aplicado

- headers e fontes do core devem conter comentários detalhados explicando responsabilidade, motivo da abordagem e intenção de manutenção de cada bloco funcional relevante;
- READMEs dos subdiretórios devem refletir essas decisões e mapear onde cada responsabilidade está implementada.

### Critérios arquiteturais desta fase

- `include/` define API pública estável;
- `src/` concentra apenas implementação interna;
- nenhuma dependência de backend gráfico no core;
- contratos pequenos, explícitos e orientados à composição;
- evolução guiada por testes unitários e de integração.

---

## Matriz de Transições do Lifecycle (baseline atual)

Esta matriz documenta o comportamento **implementado hoje** em `EngineLifecycleController`, com base em `engine/core/src/lifecycle/engine_lifecycle_controller.cpp`.

> Referência de códigos de erro:
> - `1001`: transição inválida em `initialize` (estado diferente de `Uninitialized`);
> - `1002`: `EngineConfig` inválido em `initialize`;
> - `1003`: transição inválida em `tick` (estado diferente de `Running`/`Initialized`);
> - `1004`: `FrameContext` inválido (`delta_time` negativo) em `tick`;
> - `1005`: transição inválida em `shutdown` com engine `Uninitialized`;
> - `1006`: transição inválida em `pause` (estado diferente de `Running`);
> - `1007`: transição inválida em `resume` (estado diferente de `Paused`).

| Estado atual \ Método | `initialize` | `tick` | `pause` | `resume` | `shutdown` |
|---|---|---|---|---|---|
| `Uninitialized` | **OK → `Initialized`** (se config válida) \| **erro `1002`** (config inválida, permanece `Uninitialized`) | **erro `1003`** | **erro `1006`** | **erro `1007`** | **erro `1005`** |
| `Initialized` | **erro `1001`** | **OK → `Running`** (se frame válido) \| **erro `1004`** (`delta_time` negativo; permanece `Initialized`) | **erro `1006`** | **erro `1007`** | **OK → `Stopped`** (passando internamente por `Stopping`) |
| `Running` | **erro `1001`** | **OK → `Running`** \| **erro `1004`** (`delta_time` negativo, permanece `Running`) | **OK → `Paused`** | **erro `1007`** | **OK → `Stopped`** (passando internamente por `Stopping`) |
| `Paused` | **erro `1001`** | **erro `1003`** | **erro `1006`** | **OK → `Running`** | **OK → `Stopped`** (passando internamente por `Stopping`) |
| `Stopping` | **erro `1001`** | **erro `1003`** | **erro `1006`** | **erro `1007`** | **OK → `Stopping`** (idempotente; sem mudança de estado) |
| `Stopped` | **erro `1001`** | **erro `1003`** | **erro `1006`** | **erro `1007`** | **OK → `Stopped`** (idempotente) |

### Mapeamento evento ↔ transição (attempt/success/failure)

Cada operação agora emite **sempre**:

1. `"<operação>.attempt"` no início da chamada (sem alterar estado/erro);
2. `"<operação>.success"` em finalização com sucesso;
3. `"<operação>.failure"` em finalização com erro (incluindo transição inválida).

| Método | Evento `attempt` | Evento `success` | Evento `failure` |
|---|---|---|---|
| `initialize` | `initialize.attempt` | `initialize.success` quando `Uninitialized -> Initialized` | `initialize.failure` para erro `1001` ou `1002` |
| `tick` | `tick.attempt` | `tick.success` quando frame é aceito (`Initialized -> Running` ou `Running -> Running`) | `tick.failure` para erro `1003` ou `1004` |
| `pause` | `pause.attempt` | `pause.success` quando `Running -> Paused` | `pause.failure` para erro `1006` |
| `resume` | `resume.attempt` | `resume.success` quando `Paused -> Running` | `resume.failure` para erro `1007` |
| `shutdown` | `shutdown.attempt` | `shutdown.success` quando encerra (`Initialized/Running/Paused -> Stopped`) ou no-op idempotente (`Stopping/Stopped`) | `shutdown.failure` para erro `1005` (`Uninitialized`) |

Todos os eventos carregam os campos `state_before`, `state_after`, `error_code` e `error_message`.

### Observações de semântica relevante

- `tick` chamado em `Initialized` promove o estado para `Running` **apenas após** validação de `delta_time`; em erro `1004`, o estado permanece `Initialized`.
- Em estados `Stopping` e `Stopped`, `shutdown` retorna sucesso idempotente, sem forçar nova transição.

## Idempotência (política oficial)

Esta seção define a política canônica de idempotência e erro determinístico para `IEngineLifecycle` e para a implementação padrão `EngineLifecycleController`.

- `initialize` repetido: **erro determinístico** (`code = 1001`, mensagem fixa de transição inválida).
- `tick` em estados não permitidos (`Uninitialized`, `Paused`, `Stopping`, `Stopped`): **erro determinístico** (`code = 1003`, mensagem fixa de transição inválida).
- `pause` repetido (inclui chamada em `Paused`): **erro determinístico** (`code = 1006`, mensagem fixa de transição inválida).
- `resume` repetido (inclui chamada em `Running`): **erro determinístico** (`code = 1007`, mensagem fixa de transição inválida).
- `shutdown` repetido (`Stopping`/`Stopped`): **no-op idempotente com sucesso** (`code = 0`, sem alteração de estado).

### Regra de determinismo

- Para cada violação de pré-condição de lifecycle, o par **código + mensagem** deve ser estável e reproduzível.
- A implementação não deve variar o erro por contagem de tentativa, timestamp ou contexto externo.

---

## Logging estruturado no lifecycle (contrato atual)

O `EngineLifecycleController` aceita injeção opcional de logger via construtor ou `set_logger(...)` com ponteiro não-proprietário para `IEngineLogger`.

- sem logger configurado (`nullptr`): chamadas de log são **no-op**;
- com logger configurado: `initialize`, `tick` e `shutdown` emitem evento estruturado;
- com logger configurado: todas as operações (`initialize`, `tick`, `pause`, `resume`, `shutdown`) emitem eventos de fase (`attempt`, `success`, `failure`);
- campos mínimos garantidos em cada evento de lifecycle: `state_before`, `state_after`, `error_code`, `error_message`.

### Contrato público

- interface: `engine/core/include/engine/core/contracts/i_engine_logger.hpp`;
- payload: `engine/core/include/engine/core/types/engine_logging.hpp`.

### Payload mínimo de evento

- `module` (ex.: `engine.core.lifecycle`);
- `severity` (`EngineErrorSeverity`);
- `event` em formato `<operação>.<fase>` (ex.: `initialize.attempt`, `tick.success`, `pause.failure`);
- `message`;
- `fields` chave-valor com diagnósticos.

## Gate de performance de observabilidade (`tick`)

Para evitar regressões grosseiras de custo, o core adota um microbenchmark de referência em
`tests/performance/tick_observability_benchmark.cpp` com comparação direta entre:

- `tick()` com observabilidade de frame **desabilitada** (`enable_frame_trace=false`);
- `tick()` com observabilidade de frame **habilitada** (`enable_frame_trace=true`).

### Critério de aceite (baseline inicial)

- métrica: razão média `overhead_ratio = avg_tick_on / avg_tick_off`;
- limite aceitável: `overhead_ratio <= 1.50` (até **50%** de overhead);
- comportamento de gate: execução falha quando razão excede o limite.

### Uso recomendado no gate local/CI

- executar benchmark com volume mínimo estável (ex.: `200000` iterações);
- repetir ao menos 2x e considerar o pior valor para decisão;
- bloquear merge quando qualquer execução ultrapassar o limite.

> Observação: este limite é deliberadamente conservador para a Fase 0/Fase 1 e deve ser
> reduzido conforme otimizações e maturidade da pipeline de observabilidade.

## Próximas fases / etapas

## Fase 1 — Consolidação de Contratos (curto prazo)

### Objetivo

Tornar o contrato do core mais previsível para consumo por módulos externos e aplicações host.

### Entregas previstas

- consolidar semântica de transições (`initialize`, `tick`, `pause`, `resume`, `shutdown`);
- definir catálogo padronizado de códigos de erro do lifecycle;
- formalizar contrato de idempotência (o que acontece em chamadas repetidas);
- publicar guia de uso da interface `IEngineLifecycle` com exemplos.

### Critérios de aceite

- contrato documentado com pré/pós-condições por método;
- sem ambiguidades em transições inválidas;
- exemplos de integração cobrindo fluxo nominal e erro.

## Fase 2 — Observabilidade e Telemetria do Core (médio prazo)

### Objetivo

Adicionar visibilidade operacional do runtime sem acoplamento a backend específico.

### Entregas previstas

- base de logging estruturado por módulo e severidade;
- tracing com metadados de frame (`frame_index`, `delta_time`, timestamp);
- eventos internos de lifecycle para diagnóstico;
- hooks para exportação futura a ferramentas de observabilidade.

### Critérios de aceite

- logs consistentes para bootstrap/tick/shutdown;
- tracing habilitável por configuração;
- overhead controlado quando observabilidade estiver desabilitada.

## Fase 3 — Validação de Configuração e Robustez (médio prazo)

### Objetivo

Fortalecer segurança de configuração e resiliência operacional do core.

### Entregas previstas

- pipeline de validação de `EngineConfig` com mensagens acionáveis;
- política de fallback para parâmetros inválidos;
- contrato de erro com categorização clara (recoverable vs fatal);
- testes de robustez para estados limite.

### Critérios de aceite

- validação determinística e testável;
- cobertura de cenários inválidos em testes unitários;
- documentação de comportamento de fallback.

## Fase 4 — Testes Automatizados e Gate de Qualidade (curto/médio prazo)

### Objetivo

Garantir regressão mínima e acelerar evolução segura do core.

### Entregas previstas

- suíte de testes unitários para tipos e contratos;
- suíte de integração para fluxo `bootstrap -> tick -> pause/resume -> shutdown`;
- integração no CMake/CI para execução contínua;
- baseline inicial de cobertura por módulo.

### Critérios de aceite

- testes executando local e em CI;
- cenários críticos do lifecycle cobertos;
- falhas de regressão bloqueando merge.

## Fase 5 — Preparação para Integração com Módulos Superiores (médio prazo)

### Objetivo

Transformar o core em fundação definitiva para os demais módulos do engine.

### Entregas previstas

- guias de integração para `render`, `gfx`, `tiles`, `services`, `sensors`;
- contratos de extensão (adapters e callbacks) sem dependência circular;
- versão inicial de compatibilidade semântica da API pública;
- checklist de readiness para integração cross-module.

### Critérios de aceite

- integração de pelo menos um módulo consumidor usando apenas API pública;
- ausência de dependência de backend no core;
- documentação de integração publicada e validada.

---

## Sequência recomendada de implementação

1. fechar Fase 1 (contratos e semântica);
2. implementar Fase 4 em paralelo parcial (testes para o que já existe);
3. avançar Fase 2 e Fase 3 de forma incremental;
4. concluir Fase 5 com integração guiada por testes.

## Resultado esperado ao final deste roadmap

`engine/core` atuará como fundação estável, observável e testável do runtime, reduzindo risco de retrabalho e acelerando a entrega dos módulos de alto nível do VME.
