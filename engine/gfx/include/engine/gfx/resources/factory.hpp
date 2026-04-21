/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <memory>

#include "engine/gfx/resources/interfaces.hpp"

namespace vme::engine::gfx::resources {

/**
 * @brief Resultado de criação com ownership explícito.
 *
 * Decisão de design:
 * - uso de std::unique_ptr torna dono do recurso inequívoco;
 * - facilita descarte determinístico por RAII e reduz vazamentos.
 */
struct CreateBufferResult {
    std::unique_ptr<Buffer> buffer{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(buffer); }
};

struct CreateTextureResult {
    std::unique_ptr<Texture> texture{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(texture); }
};

struct CreateSamplerResult {
    std::unique_ptr<Sampler> sampler{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(sampler); }
};

struct CreateShaderModuleResult {
    std::unique_ptr<ShaderModule> shader_module{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(shader_module); }
};

/**
 * @brief Resultado para views de buffer.
 *
 * Decisão de design:
 * - separar de views de textura melhora clareza semântica no chamador e permite
 *   validações específicas por tipo de recurso.
 */
struct CreateResourceViewFromBufferResult {
    std::unique_ptr<ResourceView> view{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(view); }
};

struct CreateResourceViewFromTextureResult {
    std::unique_ptr<ResourceView> view{};
    ResourceError error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(view); }
};

}  // namespace vme::engine::gfx::resources
