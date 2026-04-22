
// backends/backend_factory.cpp
#include "backend_factory.hpp"

#ifdef VME_GFX_BACKEND_VULKAN
#include "vme/backends/vulkan/vulkan_factory.hpp"
#endif

namespace vme::backends {

std::unique_ptr<gfx::contracts::IGfxInstance> create_default_instance() {
#ifdef VME_GFX_BACKEND_VULKAN
    return vulkan::create_vulkan_instance();
#else
    // Fallback para stub do gfx
    return gfx::contracts::create_gfx_instance_stub();
#endif
}

} // namespace