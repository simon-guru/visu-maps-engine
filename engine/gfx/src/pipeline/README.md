# engine/gfx/src/pipeline

Implementações de validação e materialização de pipelines.

## Escopo de implementação

- validação de layout e estágios de shader;
- construção de chave de cache de pipeline;
- criação de objetos de pipeline por backend via camada de tradução.

## Próximos passos do módulo

- Implementar normalização de descritores para gerar chaves de cache determinísticas.
- Adicionar camada de tradução para Vulkan/Metal/WebGL preservando o contrato público.
- Instrumentar métricas de hit/miss de cache de pipeline e tempo de criação por backend.
