# engine/gfx/include/engine/gfx/pipeline

Contratos públicos para pipeline gráfica e compute.

## Componentes de contrato

- `PipelineLayoutDesc`: slots de recursos e push constants;
- `ShaderStageDesc`: estágios habilitados e entry points;
- `GraphicsPipelineDesc`: rasterização, blend, depth/stencil, topology;
- `ComputePipelineDesc`: estágio compute e layout associado.

## Decisões de design (e por quê)

- **pipeline imutável após criação**;
  - simplifica cache e reduz estado implícito no runtime.
- **descrição declarativa completa**;
  - validação antecipada e erro mais claro para consumidor.
- **especialização opcional por capabilities**;
  - evita proliferar variantes de pipeline em backends limitados.
