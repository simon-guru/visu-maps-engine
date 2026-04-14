# engine/gfx/src

Implementação interna do módulo `engine/gfx`.

## Objetivo nesta fase

Disponibilizar stubs com validações de contrato e uma arquitetura capaz de receber
adapters Vulkan/Metal/GLES/WebGL sem alterar a API pública.

## Estratégia de implementação

1. **camada de validação**
   - valida descritores e uso de comandos antes de tocar backend.
2. **camada de tradução**
   - converte contrato abstrato em chamadas concretas por backend.
3. **camada de execução**
   - submete trabalho, coleta erros e aplica política de sincronização.

## Decisões (e por quê)

- validação centralizada reduz duplicação por backend;
- tradução isolada por adapter reduz acoplamento entre APIs;
- execução com telemetria facilita observabilidade e troubleshooting.
