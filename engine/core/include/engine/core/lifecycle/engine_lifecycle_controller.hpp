/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 02/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <mutex>

#include "engine/core/contracts/i_engine_lifecycle.hpp"

namespace vme::engine::core::lifecycle {

/**
 * @brief Implementação padrão do contrato IEngineLifecycle.
 *
 * Esta classe orquestra transições de estado válidas para bootstrap, tick e
 * shutdown mantendo o core independente de backend.
 */
class EngineLifecycleController final : public contracts::IEngineLifecycle {
public:
    EngineLifecycleController() = default;
    ~EngineLifecycleController() override = default;

    types::EngineError initialize(const types::EngineConfig& config) override;
    types::EngineError tick(const types::FrameContext& frame_context) override;
    types::EngineError shutdown() override;

    [[nodiscard]] types::EngineState state() const noexcept override;

private:
    [[nodiscard]] types::EngineError invalid_transition_error(
        std::uint32_t code,
        const char* message,
        types::EngineErrorSeverity severity = types::EngineErrorSeverity::Recoverable) const;

    mutable std::mutex mutex_ {};
    types::EngineConfig config_ {};
    types::EngineState state_ {types::EngineState::Uninitialized};
};

}  // namespace vme::engine::core::lifecycle
