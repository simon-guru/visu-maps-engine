

// backends/backend_factory.hpp
#pragma once

#include <memory>
#include <engine/gfx/contracts/i_gfx_instance.hpp>

namespace vme::backends {

[[nodiscard]] std::unique_ptr<gfx::contracts::IGfxInstance> create_default_instance();

} // namespace