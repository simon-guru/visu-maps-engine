# engine/gfx/include/engine/gfx/pipeline

Contratos públicos para pipeline gráfica e compute.

## Componentes de contrato

- `PipelineLayoutDesc`: define slots de recursos e push constants;
- `ShaderStageDesc`: descreve estágios habilitados e entry points;
- `GraphicsPipelineDesc`: consolida rasterização, attachments de cor, depth/stencil e topology;
- `ComputePipelineDesc`: descreve estágio compute e layout associado;
- `DeviceCaps`: explicita capacidades opcionais do backend para validação antecipada.

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
- Integração direta com `IGfxDevice` para criação de pipeline com cache por descritor.

## Próximos passos do módulo

- Adicionar contratos para depth attachment format separado e políticas de resolve para MSAA.
- Modelar limites por estágio (ex.: max textures por shader stage) dentro de `DeviceCaps`.
- Expor API de invalidação e métricas do cache de pipeline por backend.
