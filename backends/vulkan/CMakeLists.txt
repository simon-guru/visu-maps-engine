# backends/vulkan/CMakeLists.txt
find_package(Vulkan REQUIRED)

add_library(vme_backend_vulkan STATIC
    src/vulkan_gfx_instance.cpp
    src/vulkan_gfx_device.cpp
    src/vulkan_gfx_queue.cpp
    src/vulkan_gfx_swapchain.cpp
    src/vulkan_factory.cpp
)

target_include_directories(vme_backend_vulkan
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(vme_backend_vulkan
    PUBLIC
        engine_gfx
    PRIVATE
        ${Vulkan_LIBRARIES}
)

target_compile_features(vme_backend_vulkan PUBLIC cxx_std_20)

# Define um alias para facilitar o uso
add_library(VME::Backend ALIAS vme_backend_vulkan)