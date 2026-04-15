# engine/services

Serviços de domínio consumidos pela engine.

## Possíveis serviços

- roteamento;
- geocodificação;
- busca;
- telemetria e observabilidade;
- stream de eventos de marcadores/regiões em tempo real;
- eventos de tema/estilo para invalidação de cache de render.

## Diretriz para tempo real

Serviços devem expor eventos de domínio normalizados para consumo pelo módulo de render sem acoplamento ao provider.

## Status

Estrutura inicial com contrato de evento `ThemeStyleChangedEvent` para integração com invalidação de cache no render.
