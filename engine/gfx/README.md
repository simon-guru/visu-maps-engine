# engine/gfx

Abstrações de recursos gráficos independentes de backend.

## Objetivo nesta fase

Definir uma API gráfica mínima, consistente e estável para que o restante do engine (`render`, `tiles`, `services`) consiga emitir trabalho de GPU sem depender diretamente de Vulkan, Metal, GLES ou WebGL.

## Escopo funcional

- buffers/texturas;
- pipelines e estados gráficos;
- sincronização e comandos (em nível abstrato).

## Responsabilidades do módulo

- padronizar tipos de recurso (`Buffer`, `Texture`, `Sampler`, `ShaderModule`);
- descrever pipelines gráficas/compute de forma declarativa e validável;
- encapsular command recording (`CommandEncoder`/`CommandBuffer`) sem semântica específica de API;
- formalizar sincronização por primitives abstratas (barreiras, fences, semaphores e eventos);
- controlar ciclo de vida de recursos com ownership explícito e descarte previsível.

## Organização de diretórios proposta

```text
engine/gfx/
├── README.md
├── include/
│   ├── README.md
│   └── engine/gfx/
│       ├── resources/README.md      # buffers, textures, samplers, views
│       ├── pipeline/README.md       # pipeline layout, shader stages, states
│       ├── commands/README.md       # encoder, command buffers, submission
│       ├── sync/README.md           # barriers, fences, semaphores, timeline
│       └── contracts/README.md      # interfaces de Device/Queue/Swapchain
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

## Princípios da API (para portabilidade real)

- **API enxuta por padrão**: todo tipo novo exige justificativa de uso em ao menos dois backends.
- **Sem vazar detalhes de backend**: nada de flags/namespaces específicos de Vulkan, Metal, GLES ou WebGL na superfície pública.
- **Feature negotiation explícita**: capacidades opcionais via queries (`DeviceCaps`) e caminhos de fallback definidos.
- **Estados imutáveis onde possível**: descritores de pipeline e layouts são tratáveis como objetos de configuração estática.
- **Command model uniforme**: mesmo fluxo mental de gravação/submissão em desktop, mobile e web.

## Contratos mínimos previstos

- `IGfxInstance`: criação/destruição do contexto gráfico global;
- `IGfxDevice`: alocação de recursos, criação de pipeline e consulta de capacidades;
- `IGfxQueue`: submissão de comandos e sincronização entre filas;
- `IGfxSwapchain`: aquisição/apresentação de imagens de frame;
- `IResourceAllocator`: estratégia de alocação/subalocação desacoplada da API concreta.

## Critérios de aceitação desta etapa

- a superfície pública cabe em um conjunto pequeno de headers coesos;
- o mesmo contrato compila para backends Vulkan, Metal, GLES e WebGL sem `#ifdef` na camada de uso;
- recursos e comandos têm ciclo de vida documentado (criação, uso, descarte, erros);
- sincronização abstrata é suficiente para mapear dependências de render pass e upload;
- documentação deixa claro o que é obrigatório vs opcional por backend.

## Reavaliação

A API deve permanecer enxuta para reduzir custo de adaptação entre Vulkan, Metal, GLES e WebGL.

## Próximas entregas

1. publicar esqueleto de headers em `include/engine/gfx/*`;
2. definir matriz de capacidades por backend (mínimo comum + extensões);
3. prototipar implementação stub em `src/` com validações de contrato;
4. criar testes unitários de descritores e integração de gravação/submissão;
5. integrar primeira versão com `engine/render` para um frame básico.
