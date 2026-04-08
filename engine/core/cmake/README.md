# engine/core/cmake

Arquivos CMake específicos do módulo `engine/core`.

## Escopo desta fase

- organização de fragmentos de build do target `engine_core`;
- definição de include directories públicas e privadas;
- ligação de testes unitários e de integração do core.

## Observação

A intenção é manter o CMake modular para facilitar reutilização por apps e CI.


## Progresso atual

- target `engine_core` definido em `engine/core/CMakeLists.txt`;
- includes públicos configurados para `engine/core/include`;
- fontes iniciais: `types_engine_models.cpp` e `lifecycle/engine_lifecycle_controller.cpp`.
