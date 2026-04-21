# engine/gfx/src/pipeline

Implementações de validação e materialização de pipelines.

## Escopo de implementação

- validação de layout e estágios de shader;
- validação de attachments de cor, sample count e caps opcionais;
- construção de chave de cache de pipeline no dispositivo stub;
- criação de objetos de pipeline por backend via camada de tradução.

## Próximos passos do módulo

- Implementar chave de cache estruturada (sem hash simplificado) para evitar colisões em produção.
- Adicionar camada de tradução para Vulkan/Metal/WebGL preservando o contrato público.
- Instrumentar métricas de hit/miss de cache de pipeline e tempo de criação por backend.
