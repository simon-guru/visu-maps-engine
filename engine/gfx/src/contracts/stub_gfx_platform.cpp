/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/contracts/factory.hpp"
#include "engine/gfx/resources/backend_translation.hpp"
#include "engine/gfx/resources/validation.hpp"
#include "../resources/stub_resources.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace vme::engine::gfx::contracts {
namespace {

std::size_t hash_combine(std::size_t seed, std::size_t value) {
    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

std::size_t graphics_pipeline_key(const pipeline::GraphicsPipelineDesc& desc) {
    std::size_t seed = std::hash<std::string> {}(desc.label);
    seed = hash_combine(seed, desc.shader_stages.size());
    seed = hash_combine(seed, desc.color_attachments.size());
    seed = hash_combine(seed, desc.sample_count);

    for (const auto& stage : desc.shader_stages) {
        seed = hash_combine(seed, static_cast<std::size_t>(stage.stage));
        seed = hash_combine(seed, std::hash<std::string> {}(stage.module_name));
        seed = hash_combine(seed, std::hash<std::string> {}(stage.entry_point));
    }

    for (const auto& attachment : desc.color_attachments) {
        seed = hash_combine(seed, static_cast<std::size_t>(attachment.format));
        seed = hash_combine(seed, static_cast<std::size_t>(attachment.blend.enabled));
    }

    return seed;
}

std::size_t compute_pipeline_key(const pipeline::ComputePipelineDesc& desc) {
    std::size_t seed = std::hash<std::string> {}(desc.label);
    seed = hash_combine(seed, static_cast<std::size_t>(desc.stage.stage));
    seed = hash_combine(seed, std::hash<std::string> {}(desc.stage.module_name));
    seed = hash_combine(seed, std::hash<std::string> {}(desc.stage.entry_point));
    return seed;
}

class StubGfxSwapchain final : public IGfxSwapchain {
public:
    explicit StubGfxSwapchain(const SwapchainDesc& desc) : image_count_(desc.image_count) {}

    AcquireImageResult acquire_next_image() override {
        if (image_count_ == 0) {
            return {SwapchainErrorCode::kInvalidState, 0};
        }
        if (acquired_) {
            return {SwapchainErrorCode::kInvalidState, 0};
        }

        acquired_ = true;
        current_image_index_ = next_image_index_;
        next_image_index_ = (next_image_index_ + 1) % image_count_;
        return {SwapchainErrorCode::kOk, current_image_index_};
    }

    PresentResult present(std::uint32_t image_index) override {
        if (!acquired_ || image_index != current_image_index_) {
            return {SwapchainErrorCode::kInvalidState};
        }

        acquired_ = false;
        return {SwapchainErrorCode::kOk};
    }

    SwapchainErrorCode recreate(const SwapchainDesc& desc) override {
        if (desc.image_count < 2) {
            return SwapchainErrorCode::kInvalidArgument;
        }

        image_count_ = desc.image_count;
        next_image_index_ = 0;
        current_image_index_ = 0;
        acquired_ = false;
        return SwapchainErrorCode::kOk;
    }

private:
    std::uint32_t image_count_{2};
    std::uint32_t next_image_index_{0};
    std::uint32_t current_image_index_{0};
    bool acquired_{false};
};

class StubGfxDevice final : public IGfxDevice {
public:
    CreateQueueResult create_queue(QueueType /*queue_type*/) override {
        // Decisão:
        // - no stub, todos os tipos de fila retornam a mesma implementação para manter
        //   cobertura funcional sem multiplicar complexidade de testes.
        return {create_gfx_queue_stub(), QueueSubmitResult::ok_result()};
    }

    CreateSwapchainResult create_swapchain(const SwapchainDesc& desc) override {
        if (desc.image_count < 2) {
            return {nullptr, SwapchainErrorCode::kInvalidArgument};
        }

        return {std::make_unique<StubGfxSwapchain>(desc), SwapchainErrorCode::kOk};
    }

    [[nodiscard]] const pipeline::DeviceCaps& device_caps() const noexcept override { return caps_; }
    [[nodiscard]] const resources::ResourceCaps& resource_caps() const noexcept override {
        return resource_caps_;
    }

    pipeline::CreateGraphicsPipelineResult
    create_graphics_pipeline(const pipeline::GraphicsPipelineDesc& desc) override {
        const auto validation = pipeline::validate_graphics_pipeline_desc(desc, caps_);
        if (!validation.ok()) {
            return {0, false, validation};
        }

        const auto key = graphics_pipeline_key(desc);
        const auto it = graphics_pipeline_cache_.find(key);
        if (it != graphics_pipeline_cache_.end()) {
            return {it->second, true, pipeline::PipelineError::ok_result()};
        }

        const auto id = next_pipeline_id_++;
        graphics_pipeline_cache_[key] = id;
        return {id, false, pipeline::PipelineError::ok_result()};
    }

    pipeline::CreateComputePipelineResult
    create_compute_pipeline(const pipeline::ComputePipelineDesc& desc) override {
        const auto validation = pipeline::validate_compute_pipeline_desc(desc, caps_);
        if (!validation.ok()) {
            return {0, false, validation};
        }

        const auto key = compute_pipeline_key(desc);
        const auto it = compute_pipeline_cache_.find(key);
        if (it != compute_pipeline_cache_.end()) {
            return {it->second, true, pipeline::PipelineError::ok_result()};
        }

        const auto id = next_pipeline_id_++;
        compute_pipeline_cache_[key] = id;
        return {id, false, pipeline::PipelineError::ok_result()};
    }

    resources::CreateBufferResult create_buffer(const resources::BufferDesc& desc) override {
        // Decisão:
        // - validamos contrato + caps antes da criação para falhar cedo e evitar estado inválido.
        const auto validation = resources::validate_buffer_desc(desc, resource_caps_);
        if (!validation.ok()) {
            return {nullptr, validation};
        }
        const auto translated = resources::translate_buffer_desc(desc, backend_api_, resource_caps_);
        if (!translated.ok()) {
            return {nullptr, translated.error};
        }

        // Decisão:
        // - materialização do stub é delegada ao módulo src/resources para manter separação
        //   entre "contrato de device" e "objeto de recurso".
        return {resources::stub::create_buffer(desc), resources::ResourceError::ok_result()};
    }

    resources::CreateTextureResult create_texture(const resources::TextureDesc& desc) override {
        // Validação explícita protege contra combinações inválidas de dimensão/formato/uso.
        const auto validation = resources::validate_texture_desc(desc, resource_caps_);
        if (!validation.ok()) {
            return {nullptr, validation};
        }
        const auto translated = resources::translate_texture_desc(desc, backend_api_, resource_caps_);
        if (!translated.ok()) {
            return {nullptr, translated.error};
        }

        return {resources::stub::create_texture(desc), resources::ResourceError::ok_result()};
    }

    resources::CreateSamplerResult create_sampler(const resources::SamplerDesc& desc) override {
        // Sampler também passa por caps (ex.: anisotropia máxima por backend).
        const auto validation = resources::validate_sampler_desc(desc, resource_caps_);
        if (!validation.ok()) {
            return {nullptr, validation};
        }

        return {resources::stub::create_sampler(desc), resources::ResourceError::ok_result()};
    }

    resources::CreateShaderModuleResult create_shader_module(
        const resources::ShaderModuleDesc& desc) override {
        // Aqui ainda não há validação por "caps" de linguagem/estágio por backend;
        // mantemos apenas contrato mínimo (code/entry_point) para evolução incremental.
        const auto validation = resources::validate_shader_module_desc(desc);
        if (!validation.ok()) {
            return {nullptr, validation};
        }

        return {resources::stub::create_shader_module(desc), resources::ResourceError::ok_result()};
    }

    resources::CreateResourceViewFromBufferResult create_resource_view(
        const resources::ResourceViewDesc& desc, const resources::Buffer& buffer) override {
        // Coerência view->recurso base é validada sempre pelo descritor da própria instância base.
        const auto validation = resources::validate_resource_view_desc(desc, buffer.desc());
        if (!validation.ok()) {
            return {nullptr, validation};
        }

        return {resources::stub::create_resource_view(desc), resources::ResourceError::ok_result()};
    }

    resources::CreateResourceViewFromTextureResult create_resource_view(
        const resources::ResourceViewDesc& desc, const resources::Texture& texture) override {
        // Mesmo princípio para textura: view só nasce se range/formato forem coerentes com base.
        const auto validation = resources::validate_resource_view_desc(desc, texture.desc());
        if (!validation.ok()) {
            return {nullptr, validation};
        }

        return {resources::stub::create_resource_view(desc), resources::ResourceError::ok_result()};
    }

private:
    pipeline::DeviceCaps caps_ {
        .supports_compute = true,
        .supports_wireframe = false,
        .supports_depth_clamp = false,
        .max_color_attachments = 4,
        .max_sample_count = 4,
    };
    std::uint64_t next_pipeline_id_{1};
    std::unordered_map<std::size_t, std::uint64_t> graphics_pipeline_cache_{};
    std::unordered_map<std::size_t, std::uint64_t> compute_pipeline_cache_{};
    resources::ResourceCaps resource_caps_ {
        // Decisão:
        // - caps do stub são conservadoras e estáveis para tornar cenários de teste reproduzíveis.
        .max_buffer_size_bytes = 64ull * 1024ull * 1024ull,
        .max_texture_dimension_1d = 8192,
        .max_texture_dimension_2d = 4096,
        .max_texture_dimension_3d = 1024,
        .max_texture_array_layers = 1024,
        .max_texture_mip_levels = 12,
        .max_sampler_anisotropy = 8,
        .supports_storage_r32_float = true,
        .supports_storage_rgba16_float = false,
    };
    resources::BackendApi backend_api_{resources::BackendApi::WebGL};
};

class StubGfxInstance final : public IGfxInstance {
public:
    StubGfxInstance() { adapters_.push_back(AdapterInfo {.adapter_id = 0, .name = "stub-default-adapter"}); }

    [[nodiscard]] std::uint32_t adapter_count() const noexcept override {
        return static_cast<std::uint32_t>(adapters_.size());
    }

    [[nodiscard]] AdapterInfo adapter_info(std::uint32_t adapter_index) const noexcept override {
        if (adapter_index >= adapters_.size()) {
            return AdapterInfo {.adapter_id = 0, .name = "invalid-adapter"};
        }

        return adapters_[adapter_index];
    }

    CreateDeviceResult create_device(std::uint32_t adapter_index) override {
        if (adapter_index >= adapters_.size()) {
            return {nullptr, {QueueSubmitErrorCode::kInvalidArgument, "adapter index out of range"}};
        }

        return {std::make_unique<StubGfxDevice>(), QueueSubmitResult::ok_result()};
    }

private:
    std::vector<AdapterInfo> adapters_{};
};

}  // namespace

std::unique_ptr<IGfxDevice> create_gfx_device_stub() {
    return std::make_unique<StubGfxDevice>();
}

std::unique_ptr<IGfxInstance> create_gfx_instance_stub() {
    return std::make_unique<StubGfxInstance>();
}

}  // namespace vme::engine::gfx::contracts
