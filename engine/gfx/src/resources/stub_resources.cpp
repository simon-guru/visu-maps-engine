/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 22/04/2026
# This file is part of VISU LLC.
*/

#include "stub_resources.hpp"

#include <atomic>
#include <utility>

#include "engine/gfx/resources/debug_lifecycle.hpp"

// -----------------------------------------------------------------------------
// Variáveis globais de rastreamento (acessíveis por todas as funções neste .cpp)
// -----------------------------------------------------------------------------
namespace {

std::atomic<std::uint64_t> g_buffers_created{0};
std::atomic<std::uint64_t> g_buffers_destroyed{0};
std::atomic<std::uint64_t> g_textures_created{0};
std::atomic<std::uint64_t> g_textures_destroyed{0};
std::atomic<std::uint64_t> g_samplers_created{0};
std::atomic<std::uint64_t> g_samplers_destroyed{0};
std::atomic<std::uint64_t> g_shader_modules_created{0};
std::atomic<std::uint64_t> g_shader_modules_destroyed{0};
std::atomic<std::uint64_t> g_views_created{0};
std::atomic<std::uint64_t> g_views_destroyed{0};

}  // namespace

namespace vme::engine::gfx::resources::stub {
namespace {

// -----------------------------------------------------------------------------
// Implementações stub de recursos
// -----------------------------------------------------------------------------
class StubBuffer final : public Buffer {
public:
    explicit StubBuffer(BufferDesc desc) : desc_(std::move(desc)) { ++g_buffers_created; }
    ~StubBuffer() override { ++g_buffers_destroyed; }

    [[nodiscard]] const BufferDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint64_t size_bytes() const noexcept override { return desc_.size_bytes; }

private:
    BufferDesc desc_{};
};

class StubTexture final : public Texture {
public:
    explicit StubTexture(TextureDesc desc) : desc_(std::move(desc)) { ++g_textures_created; }
    ~StubTexture() override { ++g_textures_destroyed; }

    [[nodiscard]] const TextureDesc& desc() const noexcept override { return desc_; }
    [[nodiscard]] std::uint32_t width() const noexcept override { return desc_.width; }
    [[nodiscard]] std::uint32_t height() const noexcept override { return desc_.height; }
    [[nodiscard]] std::uint32_t depth_or_layers() const noexcept override { return desc_.depth_or_layers; }

private:
    TextureDesc desc_{};
};

class StubSampler final : public Sampler {
public:
    explicit StubSampler(SamplerDesc desc) : desc_(std::move(desc)) { ++g_samplers_created; }
    ~StubSampler() override { ++g_samplers_destroyed; }

    [[nodiscard]] const SamplerDesc& desc() const noexcept override { return desc_; }

private:
    SamplerDesc desc_{};
};

class StubShaderModule final : public ShaderModule {
public:
    explicit StubShaderModule(ShaderModuleDesc desc) : desc_(std::move(desc)) { ++g_shader_modules_created; }
    ~StubShaderModule() override { ++g_shader_modules_destroyed; }

    [[nodiscard]] const ShaderModuleDesc& desc() const noexcept override { return desc_; }

private:
    ShaderModuleDesc desc_{};
};

class StubResourceView final : public ResourceView {
public:
    explicit StubResourceView(ResourceViewDesc desc) : desc_(std::move(desc)) { ++g_views_created; }
    ~StubResourceView() override { ++g_views_destroyed; }

    [[nodiscard]] const ResourceViewDesc& desc() const noexcept override { return desc_; }

private:
    ResourceViewDesc desc_{};
};

}  // namespace

// -----------------------------------------------------------------------------
// Funções de fábrica stub
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Funções de debug para rastreamento de ciclo de vida
// -----------------------------------------------------------------------------
namespace vme::engine::gfx::resources::debug {

ResourceLifecycleStats lifecycle_stats() noexcept {
    return {
        .buffers_created          = g_buffers_created.load(),
        .buffers_destroyed        = g_buffers_destroyed.load(),
        .textures_created         = g_textures_created.load(),
        .textures_destroyed       = g_textures_destroyed.load(),
        .samplers_created         = g_samplers_created.load(),
        .samplers_destroyed       = g_samplers_destroyed.load(),
        .shader_modules_created   = g_shader_modules_created.load(),
        .shader_modules_destroyed = g_shader_modules_destroyed.load(),
        .views_created            = g_views_created.load(),
        .views_destroyed          = g_views_destroyed.load(),
    };
}

void reset_lifecycle_stats() noexcept {
    g_buffers_created          = 0;
    g_buffers_destroyed        = 0;
    g_textures_created         = 0;
    g_textures_destroyed       = 0;
    g_samplers_created         = 0;
    g_samplers_destroyed       = 0;
    g_shader_modules_created   = 0;
    g_shader_modules_destroyed = 0;
    g_views_created            = 0;
    g_views_destroyed          = 0;
}

}  // namespace vme::engine::gfx::resources::debug
