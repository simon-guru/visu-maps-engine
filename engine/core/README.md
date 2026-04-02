# engine/core

Camada base do engine, responsável por padronizar contratos, tipos fundamentais e o fluxo de lifecycle usado por todos os demais módulos.

## Objetivo nesta fase

Estabelecer a fundação técnica do engine para que `render`, `gfx`, `tiles`, `services`, `sensors` e `platform_abstraction` possam evoluir sobre uma base coesa, previsível e testável.

## Responsabilidades

- definir modelos e tipos fundamentais compartilhados;
- consolidar o lifecycle da engine (bootstrap, tick e shutdown);
- prover utilitários transversais (erros, logging, configuração e validação);
- expor contratos comuns reutilizados pelos demais módulos;
- garantir independência de backend nesta camada.

## Organização de diretórios (implementada nesta etapa)

```text
engine/core/
├── README.md
├── include/
│   ├── README.md
│   └── engine/core/
│       ├── types/README.md
│       ├── lifecycle/README.md
│       ├── contracts/README.md
│       └── utils/README.md
├── src/
│   ├── README.md
│   ├── lifecycle/README.md
│   ├── contracts/README.md
│   └── utils/README.md
├── tests/
│   ├── README.md
│   ├── unit/README.md
│   └── integration/README.md
└── cmake/
    └── README.md
```

## Critérios arquiteturais para esta fase

- `include/` contém a superfície pública estável do módulo.
- `src/` contém apenas implementação interna do core.
- nenhuma dependência de API gráfica específica (Vulkan/Metal/GLES/WebGL) deve aparecer no core.
- contratos devem ser pequenos, explícitos e orientados a composição.
- testes unitários validam tipos e contratos isolados; integração valida bootstrap/tick/shutdown.

## Fluxo de evolução recomendado

1. definir tipos base em `include/engine/core/types`;
2. formalizar estados e eventos do lifecycle em `include/engine/core/lifecycle`;
3. consolidar contratos compartilhados em `include/engine/core/contracts`;
4. implementar comportamentos em `src/*`;
5. validar em `tests/unit` e `tests/integration`.

## Próximas entregas

- primeira versão de `EngineConfig`, `EngineState` e `EngineError`;
- contrato mínimo de inicialização/encerramento (`IEngineLifecycle`);
- base de logging e tracing com metadados de frame;
- esqueleto CMake do target `engine_core`.
