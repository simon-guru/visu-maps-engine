# engine/gfx/include/engine/gfx/commands

Contratos públicos para gravação e materialização de comandos.

## Fluxo alvo

1. abrir `CommandEncoder`;
2. gravar operações (`copy`, `dispatch`, `draw`, `barrier`);
3. finalizar em `CommandBuffer` imutável;
4. submeter em `IGfxQueue`.

## Decisões de design (e por quê)

- **encoder mutável + buffer imutável**;
  - separa construção de execução e facilita rastreio/debug.
- **ordem explícita de comandos**;
  - mantém comportamento determinístico entre backends.
- **sem comandos “mágicos”**;
  - evita dependências implícitas difíceis de portar.
