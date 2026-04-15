/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 15/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

namespace vme::engine::services {

struct ThemeStyleChangedEvent {
    std::uint64_t style_revision{0};
    std::uint64_t pipeline_revision{0};
};

}  // namespace vme::engine::services
