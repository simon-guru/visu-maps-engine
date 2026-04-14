# engine/gfx/cmake

Configuração de build do módulo `engine/gfx`.

## Diretrizes

- alvo principal do módulo não depende de backend concreto;
- backends entram como alvos/adapters opcionais;
- flags de validação e instrumentação devem ser configuráveis por preset.
