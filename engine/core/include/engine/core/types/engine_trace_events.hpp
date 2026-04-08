/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>

namespace vme::engine::core::types {

/**
 * @brief Evento de trace por frame em formato estável para serialização.
 *
 * Convenção temporal:
 * - `delta_time`: duração do frame em nanossegundos (int64);
 * - `timestamp`: tempo monotônico em nanossegundos desde época do clock monotônico.
 */
struct FrameTraceEvent final {
    std::uint64_t frame_index {0U};
    std::int64_t delta_time {0};
    std::int64_t timestamp {0};
};

}  // namespace vme::engine::core::types
