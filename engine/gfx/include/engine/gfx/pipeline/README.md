# engine/gfx/include/engine/gfx/pipeline

Contratos públicos para pipeline gráfica e compute.

## Componentes de contrato

- `PipelineLayoutDesc`: define slots de recursos e push constants;
- `ShaderStageDesc`: descreve estágios habilitados e entry points;
- `GraphicsPipelineDesc`: consolida rasterização, blend, depth/stencil e topology;
- `ComputePipelineDesc`: descreve estágio compute e layout associado.

## Decisões de design (e por quê)

- **pipeline imutável após criação**;
  - simplifica cache e reduz estado implícito no runtime.
- **descrição declarativa completa**;
  - permite validação antecipada e erros mais claros para quem consome a API.
- **especialização opcional por capabilities**;
  - evita proliferar variantes de pipeline em backends limitados.

## Resultado esperado para consumidores

- Construção previsível de pipelines, com contratos estáveis entre front-end de render e backends.
- Menor custo de depuração, já que incompatibilidades aparecem na validação da descrição.

## Próximos passos do módulo

- Adicionar descritores de formato de render target e multisampling para cobrir pipelines gráficas completas.
- Introduzir estrutura explícita de capabilities (`DeviceCaps`) para validar opções opcionais por backend.
- Expor integração do contrato de pipeline com `IGfxDevice` para criação de pipelines com cache de backend.
