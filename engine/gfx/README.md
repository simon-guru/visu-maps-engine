# engine/gfx

Abstrações de recursos gráficos independentes de backend.

## Objetivo desta fase

Definir uma API gráfica mínima, consistente e estável para que o restante do engine (`render`, `tiles`, `services`) consiga emitir trabalho de GPU sem depender diretamente de Vulkan, Metal, GLES ou WebGL.

> Decisão: nesta etapa, priorizamos **contrato e semântica** antes de performance fina.
> 
> **Por quê?** Mudar contrato depois de integrar `render` e `tiles` custa muito mais do que otimizar detalhes de backend cedo demais.

## Conceitos centrais do módulo

- **Instance**: contexto global da API gráfica e enumeração de adapters.
- **Device**: dispositivo lógico com capacidades e fábricas de recursos/pipelines.
- **Queue**: fila de execução com regras de submissão e sincronização.
- **Swapchain**: fluxo de imagens de apresentação (acquire/present).
- **Resource**: objetos de GPU (buffer, texture, sampler, shader module).
- **Pipeline**: estado imutável da execução gráfica/compute.
- **CommandEncoder/CommandBuffer**: gravação e materialização de comandos.
- **Sync primitives**: barreiras e objetos de coordenação CPU↔GPU/GPU↔GPU.

## Escopo funcional

- buffers e texturas;
- pipelines e estados gráficos/compute;
- sincronização e comandos em nível abstrato;
- ciclo de vida previsível (criação, uso, descarte, erro).

## Responsabilidades do módulo

- padronizar tipos de recurso (`Buffer`, `Texture`, `Sampler`, `ShaderModule`);
- descrever pipelines de forma declarativa e validável;
- encapsular command recording (`CommandEncoder`/`CommandBuffer`) sem semântica específica de API;
- formalizar sincronização por primitives abstratas (barriers, fences, semaphores, events);
- controlar ownership e descarte explícito;
- expor negociação de features por `DeviceCaps`.

## Organização de diretórios

```text
engine/gfx/
├── README.md
├── include/
│   ├── README.md
│   └── engine/gfx/
│       ├── resources/README.md      # tipos de recurso e descritores
│       ├── pipeline/README.md       # pipeline layout, shader stages, states
│       ├── commands/README.md       # encoder, command buffer, submission
│       ├── sync/README.md           # barriers, fences, semaphores, timeline
│       └── contracts/README.md      # interfaces Instance/Device/Queue/Swapchain
├── src/
│   ├── README.md
│   ├── resources/README.md
│   ├── pipeline/README.md
│   ├── commands/README.md
│   ├── sync/README.md
│   └── contracts/README.md
├── tests/
│   ├── README.md
│   ├── unit/README.md
│   └── integration/README.md
└── cmake/
    └── README.md
```

## Princípios da API (e racional)

1. **API enxuta por padrão**
   - Nova abstração só entra quando resolve problema recorrente em pelo menos dois backends.
   - **Racional**: reduz custo de manutenção e evita “abstrações acidentais”.

2. **Sem vazar detalhes de backend**
   - Nenhuma flag, enum ou namespace público com nome de API concreta.
   - **Racional**: garante que camada consumidora não dependa de ifdefs por plataforma.

3. **Feature negotiation explícita**
   - `DeviceCaps` define o que é obrigatório, opcional e indisponível.
   - **Racional**: fallback explícito evita comportamento implícito divergente.

4. **Estados imutáveis onde possível**
   - Pipeline/layout/descritores são tratados como configuração estática.
   - **Racional**: maior previsibilidade e melhor cache em múltiplos backends.

5. **Command model uniforme**
   - Mesmo fluxo mental em desktop, mobile e web.
   - **Racional**: menor custo cognitivo para times de `render` e `tiles`.

2. **Sem vazar detalhes de backend**
   - Nenhuma flag, enum ou namespace público com nome de API concreta.
   - **Racional**: garante que camada consumidora não dependa de ifdefs por plataforma.

- `IGfxInstance`: criação/destruição do contexto global;
- `IGfxDevice`: alocação de recursos, criação de pipeline e consulta de capacidades;
- `IGfxQueue`: submissão de comandos e sincronização entre filas;
- `IGfxSwapchain`: aquisição/apresentação de imagens;
- `IResourceAllocator`: estratégia de alocação/subalocação desacoplada.

## Modelo de ciclo de vida (alto nível)

1. Criar `IGfxInstance`.
2. Selecionar adapter e criar `IGfxDevice`.
3. Criar `IGfxQueue` e `IGfxSwapchain`.
4. Alocar recursos/pipelines com validação de descritores.
5. Gravar comandos via `CommandEncoder` e finalizar em `CommandBuffer`.
6. Submeter em `IGfxQueue` com dependências de sincronização.
7. Apresentar imagem no `IGfxSwapchain`.
8. Descartar recursos com ownership explícito.

## Política de erro e validação

- **Erros de contrato**: detectados cedo (na criação de descritores/objetos).
- **Erros de execução**: retornados no submit/present com códigos categorizados.
- **Validação opcional estendida**: modo debug com checks adicionais de uso incorreto.

> Decisão: preferimos falhar cedo em validações de API a aceitar estado inválido e falhar tarde no backend.

4. **Estados imutáveis onde possível**
   - Pipeline/layout/descritores são tratados como configuração estática.
   - **Racional**: maior previsibilidade e melhor cache em múltiplos backends.

- superfície pública em poucos headers coesos;
- contrato compilável para Vulkan, Metal, GLES e WebGL sem `#ifdef` na camada de uso;
- ciclo de vida de recursos e comandos documentado;
- sincronização abstrata suficiente para dependências de render pass e upload;
- documentação explícita sobre obrigatório vs opcional por backend.

## Status de conclusão desta fase (Done)

- [x] contratos públicos mínimos definidos (`Instance`, `Device`, `Queue`, `Swapchain`, comandos e sync);
- [x] stubs determinísticos implementados para validação de semântica;
- [x] testes unitários e integrações básicas cobrindo encode/submit/present;
- [x] primeira integração com `engine/render` via caminho de frame mínimo.

## Próximo módulo

Com os critérios desta fase fechados, a evolução natural é o módulo `engine/render`
consumindo exclusivamente a API pública de `engine/gfx`.
