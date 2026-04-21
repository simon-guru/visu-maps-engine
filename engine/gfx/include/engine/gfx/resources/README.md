# engine/gfx/include/engine/gfx/resources

Contratos públicos para recursos de GPU.

## Recursos alvo

- `Buffer`: dados lineares para vertices, índices, uniforms, storage e staging;
- `Texture`: dados 1D/2D/3D, arrays, mipmaps e formatos;
- `Sampler`: política de amostragem e endereçamento;
- `ShaderModule`: binário/fonte compilada com estágio associado;
- `ResourceView`: visão tipada para binding sem expor handles de backend.

## Decisões de design (e por quê)

- **descritores explícitos** (size, usage, format, memory hints);
  - evita defaults implícitos divergentes entre Vulkan/Metal/GLES/WebGL.
- **usage flags abstratas** (CopySrc, CopyDst, Vertex, Index, Sampled, Storage...);
  - permite mapear permissões de acesso sem semântica específica de API.
- **ownership explícito**;
  - reduz vazamentos e facilita descarte determinístico.

## Validações mínimas previstas

- compatibilidade entre formato/dimensão/uso;
- alinhamento de buffer por tipo de uso;
- coerência entre view e recurso base.

## Arquivos públicos

- `resource_common.hpp`: erros e utilitários para flags em bitmask;
- `resource_desc.hpp`: descritores explícitos e enums de uso/formato;
- `interfaces.hpp`: contratos abstratos (`Buffer`, `Texture`, `Sampler`, `ShaderModule`, `ResourceView`);
- `factory.hpp`: resultados de criação com ownership explícito (`std::unique_ptr`);
- `validation.hpp`: validações inline do contrato público.

## Próximos passos

- Expandir `backend_translation` para mapeamento real de enums/flags nativas por backend (Vk*/MTL*/GL*), substituindo os códigos abstratos atuais.
- Integrar rastreamento de ciclo de vida com telemetria/diagnóstico no runtime (ex.: relatório por frame e detector opcional de leaks ativos).
- Enriquecer matriz de formatos por backend com granularidade por operação (sampled linear/filterable, storage atomics, render-target blendable).
