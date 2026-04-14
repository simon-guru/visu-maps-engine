# engine/gfx/include/engine/gfx/sync

Contratos públicos de sincronização abstrata.

## Primitives

- `Barrier`: ordenação e transição de acesso/uso de recurso;
- `Fence`: sincronização CPU ↔ GPU;
- `Semaphore`: sincronização GPU ↔ GPU entre filas/submissões;
- `Timeline`/`Event` (quando suportado): progressão incremental.

## Decisões de design (e por quê)

- **escopos e estágios abstratos** em vez de enums de backend;
  - mantém API portátil entre desktop/mobile/web.
- **dependências explícitas no submit**;
  - simplifica auditoria de hazards e evita deadlocks acidentais.
- **fallback claro para ausência de timeline**;
  - importante para perfis mais limitados (ex.: WebGL).
