# engine/core

Camada base do `visu-maps-engine`, responsável por contratos comuns, tipos fundamentais e coordenação do lifecycle que será consumido por `render`, `gfx`, `tiles`, `services`, `sensors` e `platform_abstraction`.

## Fase atual (Fase 0 — Fundação do Core)

### Objetivo

Entregar uma base mínima, estável e backend-agnóstica para permitir evolução incremental dos demais módulos sem acoplamento prematuro.

### Escopo implementado nesta fase

- modelos fundamentais: `EngineConfig`, `EngineState`, `EngineError`, `FrameContext`;
- contrato de lifecycle: `IEngineLifecycle`;
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
│       │   └── engine_models.hpp
│       ├── lifecycle/
│       │   ├── README.md
│       │   └── engine_lifecycle_controller.hpp
│       ├── contracts/
│       │   ├── README.md
│       │   └── i_engine_lifecycle.hpp
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

### Critérios arquiteturais desta fase

- `include/` define API pública estável;
- `src/` concentra apenas implementação interna;
- nenhuma dependência de backend gráfico no core;
- contratos pequenos, explícitos e orientados à composição;
- evolução guiada por testes unitários e de integração.

---

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
