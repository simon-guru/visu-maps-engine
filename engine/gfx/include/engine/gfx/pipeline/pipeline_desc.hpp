/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Contratos públicos relacionados a descritores de pipeline gráfico.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
namespace vme::engine::gfx::pipeline {

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo
 */
enum class ShaderStage : std::uint8_t {
    Vertex, // Inclui operações de renderização e computação.
    Fragment, // Inclui operações de renderização, mas não computação.
    Compute, // Inclui operações de computação, mas não renderização.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class ResourceType : std::uint8_t {
    UniformBuffer, // Inclui buffers de uniformes usados para passar dados constantes para shaders.
    StorageBuffer, // Inclui buffers de armazenamento usados para passar dados variáveis para shaders.
    SampledTexture, // Inclui texturas amostradas usadas para leitura de dados de textura.
    StorageTexture, // Inclui texturas de armazenamento usadas para escrita de dados de textura.
    Sampler, // Inclui amostradores usados para controlar a amostragem de texturas.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct ResourceSlotDesc {
    std::uint32_t set{0}; // Número do conjunto de recursos, usado para agrupamento lógico de recursos.
    std::uint32_t binding{0}; // Número do slot de recurso dentro do conjunto, usado para vinculação de recursos específicos.
    ResourceType type{ResourceType::UniformBuffer}; // Tipo de recurso, usado para determinar como o recurso será usado pelos shaders. 
    std::vector<ShaderStage> visibility{}; // Estágios de shader nos quais o recurso é visível, usado para otimização de acesso a recursos.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct PushConstantRangeDesc {
    std::uint32_t offset_bytes{0}; // Deslocamento em bytes dentro do push constant block, usado para acessar dados específicos dentro do bloco.
    std::uint32_t size_bytes{0}; // Tamanho em bytes do push constant range, usado para determinar a quantidade de dados que serão passados para os shaders.
    std::vector<ShaderStage> visibility{}; // Estágios de shader nos quais o push constant range é visível, usado para otimização de acesso a push constants.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct PipelineLayoutDesc {
    std::string label{}; // Rótulo opcional para identificação e depuração do pipeline, usado para facilitar a identificação de pipelines em logs e ferramentas de depuração.
    std::vector<ResourceSlotDesc> resource_slots{}; // Lista de descritores de slots de recursos usados para definir os recursos que serão vinculados ao pipeline, usado para configurar o layout de recursos do pipeline.
    std::vector<PushConstantRangeDesc> push_constants{}; // Lista de descritores de push constant ranges usados para definir os push constants que serão usados pelos shaders, usado para configurar o layout de push constants do pipeline.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct ShaderStageDesc {
    ShaderStage stage{ShaderStage::Vertex}; // Estágio de shader, usado para determinar o tipo de shader e como ele será usado no pipeline.
    std::string entry_point{"main"}; // Ponto de entrada do shader, usado para determinar a função principal do shader que será executada.
    std::string module_name{}; // Nome do módulo de shader, usado para identificar o módulo de shader que contém o código do shader. Pode ser usado para organização e depuração de shaders.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class PrimitiveTopology : std::uint8_t {
    TriangleList, // Inclui triângulos independentes, onde cada grupo de três vértices forma um triângulo separado.
    TriangleStrip, // Inclui triângulos conectados, onde cada vértice após os dois primeiros forma um triângulo conectado aos vértices anteriores.
    LineList, // Inclui linhas independentes, onde cada grupo de dois vértices forma uma linha separada.
    LineStrip, // Inclui linhas conectadas, onde cada vértice após o primeiro forma uma linha conectada ao vértice anterior.
    PointList, // Inclui pontos, onde cada vértice forma um ponto separado.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class PolygonMode : std::uint8_t {
    Fill, // Inclui preenchimento completo dos polígonos, onde as faces dos triângulos são preenchidas com fragmentos.
    Line, // Inclui apenas as bordas dos polígonos, onde apenas as linhas que formam os triângulos são rasterizadas.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class CullMode : std::uint8_t {
    None, // Inclui renderização de todas as faces dos triângulos, sem culling.
    Front, // Inclui culling de faces frontais, onde as faces dos triângulos que estão voltadas para a câmera são descartadas.
    Back, // Inclui culling de faces traseiras, onde as faces dos triângulos que estão voltadas para longe da câmera são descartadas.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class FrontFace : std::uint8_t {
    CounterClockwise, // Inclui definição de faces frontais como aquelas cujos vértices estão ordenados no sentido anti-horário, usado para determinar quais faces dos triângulos são consideradas frontais para fins de culling.
    Clockwise, // Inclui definição de faces frontais como aquelas cujos vértices estão ordenados no sentido horário, usado para determinar quais faces dos triângulos são consideradas frontais para fins de culling.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class BlendFactor : std::uint8_t {
    Zero, // Inclui fator de mistura que resulta em zero, usado para descartar a contribuição da fonte ou destino na operação de mistura.
    One, // Inclui fator de mistura que resulta em um, usado para manter a contribuição total da fonte ou destino na operação de mistura.
    SrcAlpha, // Inclui fator de mistura que resulta na componente alpha da fonte, usado para determinar a contribuição da fonte na operação de mistura.
    OneMinusSrcAlpha, // Inclui fator de mistura que resulta em um menos a componente alpha da fonte, usado para determinar a contribuição do destino na operação de mistura.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class BlendOp : std::uint8_t {
    Add, // Inclui operação de mistura que resulta na soma dos produtos dos fatores de mistura e das cores de origem e destino, usado para combinar as contribuições da fonte e destino na operação de mistura.
    Subtract, // Inclui operação de mistura que resulta na diferença dos produtos dos fatores de mistura e das cores de origem e destino, usado para subtrair a contribuição da fonte da contribuição do destino na operação de mistura.
    ReverseSubtract, // Inclui operação de mistura que resulta na diferença dos produtos dos fatores de mistura e das cores de origem e destino, mas na ordem inversa, usado para subtrair a contribuição do destino da contribuição da fonte na operação de mistura.
};

/**
 * @brief Descritor de configuração para blend de cor.
 * Decisão de design:
 * - struct simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct ColorBlendDesc {
    bool enabled{false}; // Indica se o blend de cor está habilitado, usado para determinar se a operação de mistura de cor deve ser aplicada durante a renderização.
    BlendFactor src_color{BlendFactor::One}; // Fator de mistura para a cor de origem, usado para determinar a contribuição da cor de origem na operação de mistura.
    BlendFactor dst_color{BlendFactor::Zero}; // Fator de mistura para a cor de destino, usado para determinar a contribuição da cor de destino na operação de mistura.
    BlendOp color_op{BlendOp::Add}; // Operação de mistura para as componentes de cor, usado para determinar como as contribuições da cor de origem e destino são combinadas na operação de mistura.
    BlendFactor src_alpha{BlendFactor::One}; // Fator de mistura para a componente alpha de origem, usado para determinar a contribuição da componente alpha de origem na operação de mistura.
    BlendFactor dst_alpha{BlendFactor::Zero}; // Fator de mistura para a componente alpha de destino, usado para determinar a contribuição da componente alpha de destino na operação de mistura.
    BlendOp alpha_op{BlendOp::Add}; //  Operação de mistura para a componente alpha, usado para determinar como as contribuições da componente alpha de origem e destino são combinadas na operação de mistura.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
enum class RenderTargetFormat : std::uint8_t {
    Rgba8Unorm, // Inclui formato de render target com componentes de cor RGBA de 8 bits sem sinal, usado para render targets comuns que não requerem alta precisão.
    Bgra8Unorm, // Inclui formato de render target com componentes de cor BGRA de 8 bits sem sinal, usado para render targets que requerem compatibilidade com formatos de cor específicos.
    Rgba16Float, // Inclui formato de render target com componentes de cor RGBA de 16 bits em ponto flutuante, usado para render targets que requerem alta precisão e faixa dinâmica.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo
 */
struct ColorAttachmentDesc {
    RenderTargetFormat format{RenderTargetFormat::Rgba8Unorm}; // Formato do render target, usado para determinar o layout de memória e a precisão das cores renderizadas.
    ColorBlendDesc blend{}; // Configuração de blend de cor, usado para determinar como as cores renderizadas são combinadas com as cores existentes no render target.
};

/**
 * @brief Sinalização de timeline após execução da submissão.
 * Decisão de design:
 * - struct simples para encapsular referência a timeline e valor a ser sinalizado;
 * - permite evolução futura para incluir múltiplas timelines ou outros mecanismos de sincronização.
 * - a estrutura `TimelineSignalInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como múltiplas timelines, tipos de sincronização, etc.) sem quebrar o contrato existente.
 * - a estrutura `TimelineSignalInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a sincronização conforme necessário.
 */
enum class CompareOp : std::uint8_t {
    Never, // Inclui comparação que nunca passa, usado para desabilitar testes de profundidade ou stencil.
    Less, // Inclui comparação que passa se a fonte for menor que o destino, usado para testes de profundidade ou stencil que requerem ordenação crescente.
    Equal, // Inclui comparação que passa se a fonte for igual ao destino, usado para testes de profundidade ou stencil que requerem igualdade.
    LessOrEqual, // Inclui comparação que passa se a fonte for menor ou igual ao destino, usado para testes de profundidade ou stencil que requerem ordenação crescente ou igualdade.
    Greater, // Inclui comparação que passa se a fonte for maior que o destino, usado para testes de profundidade ou stencil que requerem ordenação decrescente.
    NotEqual, // Inclui comparação que passa se a fonte for diferente do destino, usado para testes de profundidade ou stencil que requerem desigualdade.
    GreaterOrEqual, // Inclui comparação que passa se a fonte for maior ou igual ao destino, usado para testes de profundidade ou stencil que requerem ordenação decrescente ou igualdade.
    Always, // Inclui comparação que sempre passa, usado para desabilitar testes de profundidade ou stencil, mas ainda permitir escrita de profundidade ou stencil.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct DepthStencilDesc {
    bool depth_test_enabled{true}; // Indica se o teste de profundidade está habilitado, usado para determinar se os fragmentos devem ser testados contra o buffer de profundidade para determinar se devem ser renderizados.
    bool depth_write_enabled{true}; // Indica se a escrita no buffer de profundidade está habilitada, usado para determinar se os fragmentos que passam no teste de profundidade devem atualizar o valor de profundidade no buffer de profundidade.
    CompareOp depth_compare{CompareOp::LessOrEqual}; // Operação de comparação para o teste de profundidade, usado para determinar como os fragmentos são testados contra o buffer de profundidade.
    bool stencil_test_enabled{false}; // Indica se o teste de stencil está habilitado, usado para determinar se os fragmentos devem ser testados contra o buffer de stencil para determinar se devem ser renderizados.
    CompareOp stencil_compare{CompareOp::Always}; // Operação de comparação para o teste
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct RasterizationDesc {
    PolygonMode polygon_mode{PolygonMode::Fill}; // Modo de rasterização, usado para determinar como os triângulos são rasterizados (preenchidos, apenas bordas, etc.).
    CullMode cull_mode{CullMode::Back}; // Modo de culling, usado para determinar quais faces dos triângulos são descartadas durante a rasterização.
    FrontFace front_face{FrontFace::CounterClockwise}; // Definição de faces frontais, usado para determinar quais faces dos triângulos são consideradas frontais para fins de culling.
    bool depth_clamp_enabled{false}; // Indica se o depth clamp está habilitado, usado para determinar se os fragmentos que estão além do plano de corte de profundidade devem ser clamped em vez de descartados.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct GraphicsPipelineDesc {
    std::string label{}; // Rótulo opcional para identificação e depuração do pipeline, usado para facilitar a identificação de pipelines em logs e ferramentas de depuração.
    PipelineLayoutDesc layout{}; // Descritor de layout do pipeline, usado para configurar o layout de recursos e push constants do pipeline.
    std::vector<ShaderStageDesc> shader_stages{}; // Lista de descritores de estágios de shader usados para definir os shaders que serão usados no pipeline, usado para configurar os estágios de shader do pipeline.
    PrimitiveTopology topology{PrimitiveTopology::TriangleList}; // Topologia de primitiva, usado para determinar como os vértices são interpretados para formar primitivas durante a rasterização.
    RasterizationDesc rasterization{}; // Configuração de rasterização, usado para determinar como os triângulos são rasterizados e quais faces são descartadas.
    std::vector<ColorAttachmentDesc> color_attachments{}; // Lista de descritores de anexos de cor usados para definir os render targets que serão usados no pipeline, usado para configurar os render targets do pipeline.
    DepthStencilDesc depth_stencil{}; // Configuração de profundidade e stencil, usado para determinar como os testes de profundidade e stencil são realizados durante a renderização.
    std::uint8_t sample_count{1}; // Contagem de amostras para render targets, usado para determinar o nível de anti-aliasing aplicado durante a renderização. Um valor maior que 1 indica que o pipeline suporta render targets multi-amostrados.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct ComputePipelineDesc {
    std::string label{}; // Rótulo opcional para identificação e depuração do pipeline, usado para facilitar a identificação de pipelines em logs e ferramentas de depuração.
    PipelineLayoutDesc layout{}; // Descritor de layout do pipeline, usado para configurar o layout de recursos e push constants do pipeline.
    ShaderStageDesc stage{.stage = ShaderStage::Compute}; // Descritor de estágio de shader usado para definir o shader de computação que será usado no pipeline, usado para configurar o estágio de shader do pipeline.
};

/**
 * @brief Descritores de configuração para criação de pipelines gráficos e de computação.
 * Decisão de design:
 * - structs simples para encapsular descritores de configuração;
 * - enums fechados para manter contrato estável entre implementações;
 * - evita propagar códigos de erro específicos de backend para a API pública.
 * - as estruturas de descritores são projetadas para serem facilmente extensíveis no futuro, permitindo a adição de campos adicionais (como novos tipos de recursos, estágios de shader, etc.) sem quebrar o contrato existente.
 * - as estruturas de descritores são projetadas para serem facilmente integráveis com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre a configuração do pipeline conforme necessário.
 */
struct DeviceCaps {
    bool supports_compute{true}; // Indica se o dispositivo suporta pipelines de computação, usado para determinar se a criação de pipelines de computação é permitida.
    bool supports_wireframe{false}; // Indica se o dispositivo suporta rasterização em modo wireframe, usado para determinar se a configuração de rasterização com PolygonMode::Line é permitida.
    bool supports_depth_clamp{false}; // Indica se o dispositivo suporta depth clamp, usado para determinar se a configuração de rasterização com depth_clamp_enabled = true é permitida.
    std::uint8_t max_color_attachments{4}; // Número máximo de anexos de cor suportados, usado para determinar o tamanho máximo da lista de color_attachments em GraphicsPipelineDesc.
    std::uint8_t max_sample_count{1}; // Contagem máxima de amostras suportada para render targets, usado para determinar o valor máximo permitido para sample_count em GraphicsPipelineDesc.
};

}  // namespace vme::engine::gfx::pipeline
