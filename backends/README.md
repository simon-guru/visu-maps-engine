# backends

Implementações de render backend por API gráfica.

## Backends previstos

- `vulkan/`
- `metal/`
- `gles/`
- `gl46/`
- `webgl/`

## Funcionalidades alvo dos backends

- inicialização de contexto/device;
- criação e atualização de buffers/texturas;
- submissão de comandos para draw básico de mapa;
- sincronização de frame e apresentação.

## Diretriz

Manter paridade funcional mínima entre backends para features core de mapa.
