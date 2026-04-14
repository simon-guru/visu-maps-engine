# engine/gfx/include/engine/gfx/commands

Contratos públicos para gravação e materialização de comandos.

## Fluxo alvo

1. abrir `ICommandEncoder`;
2. gravar operações explícitas (`copy`, `dispatch`, `draw`, `barrier`);
3. finalizar em `ICommandBuffer` imutável via `finish`;
4. submeter o buffer finalizado em `IGfxQueue`.

## Headers implementados

- `command_common.hpp`: tipos base de comandos, descritores e erros;
- `i_command_encoder.hpp`: contrato mutável de gravação;
- `i_command_buffer.hpp`: contrato imutável de execução;
- `engine/gfx/commands.hpp`: include estável para consumidores.

## Decisões de design (e por quê)

- **encoder mutável + command buffer imutável**;
  - separa construção de execução e facilita rastreio/debug.
- **ordem explícita de comandos**;
  - mantém comportamento determinístico entre backends.
- **sem comandos “mágicos”**;
  - evita dependências implícitas difíceis de portar.
