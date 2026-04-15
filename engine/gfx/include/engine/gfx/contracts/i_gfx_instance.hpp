/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <memory>

#include "engine/gfx/contracts/i_gfx_device.hpp"

namespace vme::engine::gfx::contracts {

struct AdapterInfo {
    std::uint32_t adapter_id{0};
    const char* name{"stub-adapter"};
};

struct CreateDeviceResult {
    std::unique_ptr<IGfxDevice> device{};
    QueueSubmitResult error{};

    [[nodiscard]] bool ok() const noexcept { return error.ok() && static_cast<bool>(device); }
};

class IGfxInstance {
public:
    virtual ~IGfxInstance() = default;

    [[nodiscard]] virtual std::uint32_t adapter_count() const noexcept = 0;
    [[nodiscard]] virtual AdapterInfo adapter_info(std::uint32_t adapter_index) const noexcept = 0;
    virtual CreateDeviceResult create_device(std::uint32_t adapter_index) = 0;
};

}  // namespace vme::engine::gfx::contracts
