/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

#include "engine/gfx/resources/resource_desc.hpp"

namespace vme::engine::gfx::resources {

/**
 * @brief Contrato mínimo de buffer.
 *
 * Decisão de design:
 * - expõe apenas metadados estáveis (descritor/tamanho);
 * - não expõe ponteiros ou handles nativos para preservar portabilidade.
 */
class Buffer {
public:
    virtual ~Buffer() = default;

    [[nodiscard]] virtual const BufferDesc& desc() const noexcept = 0;
    [[nodiscard]] virtual std::uint64_t size_bytes() const noexcept = 0;
};

class Texture {
public:
    virtual ~Texture() = default;

    [[nodiscard]] virtual const TextureDesc& desc() const noexcept = 0;
    [[nodiscard]] virtual std::uint32_t width() const noexcept = 0;
    [[nodiscard]] virtual std::uint32_t height() const noexcept = 0;
    [[nodiscard]] virtual std::uint32_t depth_or_layers() const noexcept = 0;
};

class Sampler {
public:
    virtual ~Sampler() = default;

    [[nodiscard]] virtual const SamplerDesc& desc() const noexcept = 0;
};

class ShaderModule {
public:
    virtual ~ShaderModule() = default;

    [[nodiscard]] virtual const ShaderModuleDesc& desc() const noexcept = 0;
};

/**
 * @brief Visão de binding sobre recurso base.
 *
 * Decisão de design:
 * - separar "recurso" de "visão" permite reinterpretar subconjuntos (mips/layers/ranges)
 *   sem quebrar encapsulamento de backend.
 */
class ResourceView {
public:
    virtual ~ResourceView() = default;

    [[nodiscard]] virtual const ResourceViewDesc& desc() const noexcept = 0;
};

}  // namespace vme::engine::gfx::resources
