# engine/gfx

Abstrações de recursos gráficos independentes de backend.

## Escopo

- buffers/texturas;
- pipelines e estados gráficos;
- sincronização e comandos (em nível abstrato).

## Reavaliação

A API deve permanecer enxuta para reduzir custo de adaptação entre Vulkan, Metal, GLES e WebGL.
