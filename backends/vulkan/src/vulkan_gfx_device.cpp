

#include "backends/vulkan/vulkan_gfx_device.hpp"

namespace vme::backends::vulkan {

gfx::contracts::CreateQueueResult VulkanGfxDevice::create_queue(gfx::contracts::QueueType type) {
    uint32_t family_index = UINT32_MAX;
    switch (type) {
        case gfx::contracts::QueueType::Graphics: family_index = graphics_queue_family_; break;
        case gfx::contracts::QueueType::Compute:  family_index = compute_queue_family_; break;
        case gfx::contracts::QueueType::Transfer: family_index = transfer_queue_family_; break;
    }
    if (family_index == UINT32_MAX) {
        return { nullptr, { gfx::contracts::QueueSubmitErrorCode::kInvalidArgument, "Queue type not supported" } };
    }
    VkQueue vk_queue;
    vkGetDeviceQueue(device_, family_index, 0, &vk_queue);
    auto queue = std::make_unique<VulkanGfxQueue>(*this, vk_queue, family_index);
    return { std::move(queue), gfx::contracts::QueueSubmitResult::ok_result() };
}

gfx::pipeline::CreateGraphicsPipelineResult VulkanGfxDevice::create_graphics_pipeline(
    const gfx::pipeline::GraphicsPipelineDesc& desc) {

    // 1. Validar descritor com as funções do gfx
    auto validation_error = gfx::pipeline::validate_graphics_pipeline_desc(desc, caps_);
    if (!validation_error.ok()) {
        return { 0, false, { gfx::pipeline::PipelineErrorCode::InvalidArgument, validation_error.message } };
    }

    // 2. Mapear shaders (precisamos de um cache de VkShaderModule)
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    for (const auto& stage : desc.shader_stages) {
        // Procurar o shader module previamente criado (pelo module_name)
        // Exemplo simplificado: assumimos que existe um mapa de módulos.
        // ...
    }

    // 3. Configurar estados fixos
    VkPipelineVertexInputStateCreateInfo vertex_input{};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = to_vk_topology(desc.topology);

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = to_vk_polygon_mode(desc.rasterization.polygon_mode);
    rasterizer.cullMode = to_vk_cull_mode(desc.rasterization.cull_mode);
    rasterizer.frontFace = to_vk_front_face(desc.rasterization.front_face);
    rasterizer.depthClampEnable = desc.rasterization.depth_clamp_enabled;

    // 4. Configurar blend e depth/stencil
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;
    for (const auto& att : desc.color_attachments) {
        VkPipelineColorBlendAttachmentState blend{};
        blend.blendEnable = att.blend.enabled;
        blend.srcColorBlendFactor = to_vk_blend_factor(att.blend.src_color);
        blend.dstColorBlendFactor = to_vk_blend_factor(att.blend.dst_color);
        blend.colorBlendOp = to_vk_blend_op(att.blend.color_op);
        blend.srcAlphaBlendFactor = to_vk_blend_factor(att.blend.src_alpha);
        blend.dstAlphaBlendFactor = to_vk_blend_factor(att.blend.dst_alpha);
        blend.alphaBlendOp = to_vk_blend_op(att.blend.alpha_op);
        blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachments.push_back(blend);
    }

    VkPipelineColorBlendStateCreateInfo color_blend{};
    color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.attachmentCount = static_cast<uint32_t>(color_blend_attachments.size());
    color_blend.pAttachments = color_blend_attachments.data();

    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = desc.depth_stencil.depth_test_enabled;
    depth_stencil.depthWriteEnable = desc.depth_stencil.depth_write_enabled;
    depth_stencil.depthCompareOp = to_vk_compare_op(desc.depth_stencil.depth_compare);

    // 5. Criar pipeline layout (a partir de desc.layout)
    VkPipelineLayout pipeline_layout = create_pipeline_layout(desc.layout);

    // 6. Criar render pass compatível (aqui usaremos um render pass dummy para testes)
    VkRenderPass render_pass = get_or_create_compatible_render_pass(desc);

    // 7. Montar e criar o pipeline
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pColorBlendState = &color_blend;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
        return { 0, false, { gfx::pipeline::PipelineErrorCode::BackendFailure, "vkCreateGraphicsPipelines failed" } };
    }

    // Gerar um ID único (pode ser o handle ou um contador)
    static std::uint64_t next_id = 1;
    std::uint64_t id = next_id++;
    pipelines_[id] = { pipeline, pipeline_layout }; // armazenar para uso futuro

    return { id, false, gfx::pipeline::PipelineError::ok_result() };
}




gfx::pipeline::CreateGraphicsPipelineResult VulkanGfxDevice::create_graphics_pipeline(
    const gfx::pipeline::GraphicsPipelineDesc& desc) {

    // 1. Validação usando as funções do gfx
    auto validation = gfx::pipeline::validate_graphics_pipeline_desc(desc, caps_);
    if (!validation.ok()) {
        return { 0, false, { gfx::pipeline::PipelineErrorCode::InvalidArgument, validation.message } };
    }

    // 2. Coletar estágios de shader
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    for (const auto& stage_desc : desc.shader_stages) {
        VkShaderModule module = get_or_create_shader_module(stage_desc.module_name, /*código*/ {});
        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.stage = to_vk_shader_stage(stage_desc.stage);
        stage.module = module;
        stage.pName = stage_desc.entry_point.c_str();
        stages.push_back(stage);
    }

    // 3. Configurar estados fixos (vertex input, assembly, rasterização, etc.)
    VkPipelineVertexInputStateCreateInfo vertex_input{};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // sem atributos por enquanto

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = to_vk_topology(desc.topology);

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = to_vk_polygon_mode(desc.rasterization.polygon_mode);
    rasterizer.cullMode = to_vk_cull_mode(desc.rasterization.cull_mode);
    rasterizer.frontFace = to_vk_front_face(desc.rasterization.front_face);
    rasterizer.depthClampEnable = desc.rasterization.depth_clamp_enabled;
    rasterizer.lineWidth = 1.0f;

    // 4. Blend e Depth/Stencil
    std::vector<VkPipelineColorBlendAttachmentState> blend_attachments;
    for (const auto& att : desc.color_attachments) {
        VkPipelineColorBlendAttachmentState blend{};
        blend.blendEnable = att.blend.enabled ? VK_TRUE : VK_FALSE;
        blend.srcColorBlendFactor = to_vk_blend_factor(att.blend.src_color);
        blend.dstColorBlendFactor = to_vk_blend_factor(att.blend.dst_color);
        blend.colorBlendOp = to_vk_blend_op(att.blend.color_op);
        blend.srcAlphaBlendFactor = to_vk_blend_factor(att.blend.src_alpha);
        blend.dstAlphaBlendFactor = to_vk_blend_factor(att.blend.dst_alpha);
        blend.alphaBlendOp = to_vk_blend_op(att.blend.alpha_op);
        blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                               VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend_attachments.push_back(blend);
    }

    VkPipelineColorBlendStateCreateInfo color_blend{};
    color_blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend.attachmentCount = static_cast<uint32_t>(blend_attachments.size());
    color_blend.pAttachments = blend_attachments.data();

    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = desc.depth_stencil.depth_test_enabled;
    depth_stencil.depthWriteEnable = desc.depth_stencil.depth_write_enabled;
    depth_stencil.depthCompareOp = to_vk_compare_op(desc.depth_stencil.depth_compare);

    // 5. Criar pipeline layout
    VkPipelineLayout layout = create_pipeline_layout(desc.layout);

    // 6. Obter render pass compatível (para simplificar, um render pass dummy)
    VkRenderPass render_pass = get_or_create_compatible_render_pass(desc);

    // 7. Criar pipeline
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = static_cast<uint32_t>(stages.size());
    pipeline_info.pStages = stages.data();
    pipeline_info.pVertexInputState = &vertex_input;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pColorBlendState = &color_blend;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.layout = layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
        return { 0, false, { gfx::pipeline::PipelineErrorCode::BackendFailure, "vkCreateGraphicsPipelines failed" } };
    }

    // Gerar ID
    static std::uint64_t next_id = 1;
    std::uint64_t id = next_id++;
    pipelines_[id] = { pipeline, layout };

    return { id, false, gfx::pipeline::PipelineError::ok_result() };
}



} // namespace vme::backends::vulkan