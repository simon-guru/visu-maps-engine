/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#include "stub_resources.hpp"

#include <utility>

namespace vme::engine::gfx::resources::stub {
namespace {

// Decisão:
// - objetos stub armazenam apenas o descritor original para manter comportamento previsível
//   e totalmente determinístico em testes, sem simular alocação real de GPU.
class StubBuffer final : public Buffer {
public:
    explicit StubBuffer(BufferDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const BufferDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint64_t size_bytes() const noexcept override { return desc_.size_bytes; }

private:
    BufferDesc desc_{};
};

class StubTexture final : public Texture {
public:
    explicit StubTexture(TextureDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const TextureDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint32_t width() const noexcept override { return desc_.width; }
    [[nodiscard]] std::uint32_t height() const noexcept override { return desc_.height; }
    [[nodiscard]] std::uint32_t depth_or_layers() const noexcept override { return desc_.depth_or_layers; }

private:
    TextureDesc desc_{};
};

class StubSampler final : public Sampler {
public:
    explicit StubSampler(SamplerDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const SamplerDesc& desc() const noexcept override { return desc_; }

private:
    SamplerDesc desc_{};
};

class StubShaderModule final : public ShaderModule {
public:
    explicit StubShaderModule(ShaderModuleDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const ShaderModuleDesc& desc() const noexcept override { return desc_; }

private:
    ShaderModuleDesc desc_{};
};

class StubResourceView final : public ResourceView {
public:
    explicit StubResourceView(ResourceViewDesc desc) : desc_(std::move(desc)) {}

    [[nodiscard]] const ResourceViewDesc& desc() const noexcept override { return desc_; }

private:
    ResourceViewDesc desc_{};
};

}  // namespace

// Decisão:
// - cada função de criação transfere ownership via std::unique_ptr para espelhar o
//   contrato público esperado pelos backends reais.
std::unique_ptr<Buffer> create_buffer(BufferDesc desc) {
    return std::make_unique<StubBuffer>(std::move(desc));
}

std::unique_ptr<Texture> create_texture(TextureDesc desc) {
    return std::make_unique<StubTexture>(std::move(desc));
}

std::unique_ptr<Sampler> create_sampler(SamplerDesc desc) {
    return std::make_unique<StubSampler>(std::move(desc));
}

std::unique_ptr<ShaderModule> create_shader_module(ShaderModuleDesc desc) {
    return std::make_unique<StubShaderModule>(std::move(desc));
}

std::unique_ptr<ResourceView> create_resource_view(ResourceViewDesc desc) {
    return std::make_unique<StubResourceView>(std::move(desc));
}

}  // namespace vme::engine::gfx::resources::stub
