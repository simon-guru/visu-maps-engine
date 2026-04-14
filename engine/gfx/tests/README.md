# engine/gfx/tests

Estratégia de testes para o módulo `engine/gfx`.

## Objetivos

- garantir estabilidade da API pública;
- validar regras de contrato e erros esperados;
- assegurar comportamento equivalente entre backends.

## Pirâmide de testes

1. **Unitários**: validação de descritores, enums, regras de uso.
2. **Integração**: fluxo encoder → submit → sync → present (com stubs/mocks).
3. **Conformidade por backend**: mesma suite sobre adapters concretos.

## Decisão de qualidade

Toda feature nova deve nascer com:

- teste unitário de validação de contrato;
- caso de integração cobrindo caminho feliz e pelo menos um erro esperado.
