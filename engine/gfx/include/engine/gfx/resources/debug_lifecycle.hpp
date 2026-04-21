/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

namespace vme::engine::gfx::resources::debug {

struct ResourceLifecycleStats {
    std::uint64_t buffers_created{0};
    std::uint64_t buffers_destroyed{0};
    std::uint64_t textures_created{0};
    std::uint64_t textures_destroyed{0};
    std::uint64_t samplers_created{0};
    std::uint64_t samplers_destroyed{0};
    std::uint64_t shader_modules_created{0};
    std::uint64_t shader_modules_destroyed{0};
    std::uint64_t views_created{0};
    std::uint64_t views_destroyed{0};
};

[[nodiscard]] ResourceLifecycleStats lifecycle_stats() noexcept;
void reset_lifecycle_stats() noexcept;

}  // namespace vme::engine::gfx::resources::debug
