# visu-maps-engine

## Status de implementação do engine-core

Em **02/04/2026**, o módulo `engine/core` passou a incluir implementação inicial de código em C++ para:

- modelos fundamentais (`EngineConfig`, `EngineState`, `EngineError`, `FrameContext`);
- contrato de lifecycle (`IEngineLifecycle`);
- implementação padrão de lifecycle (`EngineLifecycleController`);
- target CMake `engine_core`.

A documentação técnica dessas declarações e implementações está nos READMEs internos de `engine/core/include/*` e `engine/core/src/*`.


## Funcionalidades

O **VME (visu-maps-engine)** pode ser configurado para operar como uma plataforma de mapas personalizável e orientada a eventos em tempo real.

### 1) Marcadores dinâmicos em tempo real
- Marcadores para **regiões, pessoas, objetos e pontos de interesse (POIs)**.
- Categorias sugeridas: **mercados, farmácias, postos médicos, postos policiais, estações** e outras conforme o domínio.
- Atualização em tempo real via stream de eventos (ex.: WebSocket/MQTT/Kafka).
- Estados visuais por marcador (online/offline, alerta, emergência, indisponível).

### 2) Camadas (layers) personalizáveis por cliente
- Ativação/desativação de camadas por perfil/tenant.
- Temas por cliente (cores, ícones, rótulos e filtros).
- Organização por contexto de negócio: segurança, saúde, mobilidade e comércio.

### 3) Geofencing e alertas
- Definição de áreas monitoradas (polígonos/círculos).
- Regras de entrada/saída e permanência.
- Geração de notificações para operações críticas.

### 4) Busca e filtros avançados
- Busca textual e geográfica (ex.: “farmácia 24h perto de mim”).
- Filtros por distância, tipo, status e disponibilidade.
- Ordenação por distância, prioridade operacional e ETA.

### 5) Roteamento e apoio operacional
- Suporte a múltiplos modos (a pé, carro, bike e transporte público).
- Estratégias de rota: mais rápida, mais segura e menor custo.
- Visualização de trilhas e histórico de deslocamento.

### 6) Segurança e governança
- Controle de acesso por perfil (RBAC/ABAC).
- Isolamento por tenant para clientes distintos.
- Observabilidade de eventos e trilhas de auditoria.

## Integrações com os apps

A estrutura do repositório já separa aplicações por plataforma, permitindo reutilizar o mesmo núcleo do engine com adaptações de interface e integração em cada app.

### App Web (`apps/web`)
- Ideal para painéis operacionais e monitoramento em tempo real.
- Integração com APIs do VME para renderizar layers, marcadores e alertas.
- Consumo de eventos em tempo real (ex.: WebSocket) para atualizar o mapa sem recarregar.

### App Android (`apps/android`)
- Uso em mobilidade (equipes de campo e operações externas).
- Integração com localização do dispositivo e envio de telemetria.
- Suporte a notificações de proximidade (geofence) e status operacional.

### App iOS (`apps/ios`)
- Cenários equivalentes ao Android para operações móveis.
- Integração com recursos nativos de localização e notificações.
- Foco em experiência fluida para acompanhamento de ativos e ocorrências.

### App Desktop Qt (`apps/desktop-qt`)
- Operação em centrais de comando e ambientes corporativos.
- Visualização de múltiplas camadas e dados de alto volume em telas maiores.
- Integração com ferramentas internas via APIs e serviços corporativos.

## Estratégia de integração recomendada

1. **API única do VME** para cadastro e consulta de entidades geográficas.
2. **Canal de eventos em tempo real** para sincronizar estado entre apps.
3. **Contrato de dados padronizado** (JSON/GeoJSON) para interoperabilidade.
4. **Feature flags por app/tenant** para liberar funcionalidades de forma incremental.
5. **Autenticação centralizada** com autorização por perfil e contexto.

## Integração da feature de toque/clique em marcadores e visualização de perfil

Esta feature permite que qualquer marcador no mapa (região, pessoa, posto hospitalar, mercado, estação etc.) responda a interação do usuário e abra uma visualização com informações detalhadas do objeto selecionado.

### Fluxo funcional (alto nível)

1. O app renderiza marcadores com `id`, `tipo`, `posição` e `estado`.
2. O usuário realiza **toque** (mobile) ou **clique** (web/desktop) no marcador.
3. O app dispara evento `marker.selected` para o módulo de interação.
4. O app consulta o perfil detalhado no backend (`GET /entities/{id}` ou endpoint por tipo).
5. O app abre um componente de detalhe (bottom sheet, modal, drawer ou painel lateral).
6. O app mantém sincronização em tempo real para atualizar o perfil enquanto ele estiver aberto.

### Contrato mínimo do marcador

```json
{
  "id": "entity_123",
  "type": "hospital",
  "coordinates": { "lat": -23.55, "lng": -46.63 },
  "status": "online",
  "title": "Hospital Central",
  "layer": "saude"
}
```

### Contrato mínimo do perfil detalhado

```json
{
  "id": "entity_123",
  "type": "hospital",
  "name": "Hospital Central",
  "status": "operando",
  "address": "Rua Exemplo, 100",
  "contacts": [{ "label": "Recepção", "value": "+55 11 99999-9999" }],
  "services": ["emergência", "UTI", "pediatria"],
  "last_update": "2026-04-02T10:30:00Z",
  "metadata": {
    "capacity": 120,
    "occupancy": 87,
    "priority": "alta"
  }
}
```

### Padrões de UI recomendados para exibir informações

- **Web/Desktop**: painel lateral com tabs (Resumo, Contato, Histórico, Ações).
- **Android/iOS**: bottom sheet expansível com ações rápidas (ligar, navegar, compartilhar).
- **Regiões (polígono)**: cabeçalho com nome da área + métricas agregadas (ocorrências, ativos na área, risco).
- **Pessoas/objetos móveis**: card com status em tempo real, velocidade, direção e trilha recente.

### Integração por plataforma

#### App Web (`apps/web`)
- Registrar listener de clique no layer de marcadores.
- Resolver conflitos entre cluster e item individual (expandir cluster antes de selecionar entidade).
- Abrir drawer lateral com carregamento progressivo (skeleton + dados em streaming).

#### App Android (`apps/android`)
- Tratar `tap` curto para selecionar e `long press` para ações contextuais.
- Exibir bottom sheet e manter estado em rotação/retorno de background.
- Integrar com intents nativas (telefone, rota, compartilhamento).

#### App iOS (`apps/ios`)
- Mesma semântica de interação do Android para consistência de produto.
- Usar painel/modal nativo e haptics para feedback de seleção.
- Atualizar o perfil em foreground com eventos de realtime.

#### App Desktop Qt (`apps/desktop-qt`)
- Suporte a clique simples para seleção e duplo clique para foco/zoom.
- Painel lateral persistente com detalhes e ações operacionais.
- Atalhos de teclado para alternar entidades e layers.

### Regras de negócio sugeridas

- Permissões por perfil: usuário só visualiza atributos autorizados daquele marcador.
- Fallback offline: mostrar último snapshot local com indicador de desatualização.
- Timeout e retry para carregamento do perfil detalhado.
- Telemetria de UX: medir tempo de abertura, taxa de erro e uso por tipo de marcador.

### Eventos de integração recomendados

- `marker.selected` (entrada: `id`, `type`, `source_app`, `timestamp`)
- `profile.opened` (entrada: `id`, `latency_ms`)
- `profile.updated` (entrada: `id`, `changed_fields`)
- `profile.action_clicked` (entrada: `id`, `action`)

Com esse fluxo, o VME garante uma experiência uniforme de seleção de marcadores e consulta de perfil em todos os apps, mantendo consistência de dados, segurança e atualização em tempo real.

## Mini contrato de integração (marker -> perfil)

Abaixo está um contrato mínimo para implementação consistente no motor e consumo em Kotlin (mobile) e Qt (desktop).

### 1) Contrato de domínio

```ts
// Tipo base para qualquer marcador selecionável
export type MarkerRef = {
  id: string;
  type: "region" | "person" | "hospital" | "pharmacy" | "police" | "station" | "market" | "object";
  lat: number;
  lng: number;
  layer: string;
};

// Perfil retornado pelo backend
export type EntityProfile = {
  id: string;
  type: string;
  name: string;
  status: string;
  updatedAt: string; // ISO-8601
  summary?: string;
  contacts?: Array<{ label: string; value: string }>;
  metadata?: Record<string, string | number | boolean>;
};
```

### 2) Contrato de API HTTP

- `GET /entities/{id}`
  - **200**: `EntityProfile`
  - **404**: entidade não encontrada
  - **403**: sem permissão

Exemplo de resposta:

```json
{
  "id": "entity_123",
  "type": "hospital",
  "name": "Hospital Central",
  "status": "operando",
  "updatedAt": "2026-04-02T11:00:00Z",
  "summary": "Atendimento 24h",
  "contacts": [{ "label": "Recepção", "value": "+55 11 99999-9999" }],
  "metadata": { "capacity": 120, "occupancy": 87 }
}
```

### 3) Contrato de eventos

- `marker.selected`
  - payload: `{ id, type, sourceApp, timestamp }`
- `profile.opened`
  - payload: `{ id, latencyMs }`
- `profile.updated`
  - payload: `{ id, changedFields }`
- `profile.error`
  - payload: `{ id, code, message }`

### 4) Interface mínima do motor (SDK)

```ts
export interface MarkerProfileEngine {
  selectMarker(ref: MarkerRef): Promise<void>;          // dispara marker.selected
  loadProfile(id: string): Promise<EntityProfile>;      // chama API
  subscribeProfile(id: string, cb: (p: EntityProfile) => void): () => void; // realtime
}
```

### 5) Mapeamento de uso por plataforma

- **Kotlin (Android/iOS compartilhado de domínio):**
  - `selectMarker()` no tap do marcador.
  - `loadProfile()` para preencher BottomSheet.
  - `subscribeProfile()` enquanto a tela de detalhe estiver aberta.

- **Qt (Desktop):**
  - `selectMarker()` no clique simples.
  - `loadProfile()` para painel lateral.
  - `subscribeProfile()` para atualizar campos sem refresh manual.

### 6) Regras obrigatórias

1. Sempre validar autorização antes de retornar perfil.
2. Retornar snapshot local se offline (`stale=true`).
3. Instrumentar latência de abertura de perfil (`profile.opened`).
4. Garantir compatibilidade retroativa de payload de eventos.
