find_package(Vulkan REQUIRED)

add_library(vme_backend_vulkan STATIC
    src/vulkan_gfx_instance.cpp
    src/vulkan_gfx_device.cpp
    src/vulkan_gfx_queue.cpp
    src/vulkan_gfx_swapchain.cpp
    src/vulkan_command_encoder.cpp
    src/vulkan_resources.cpp
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
target_compile_definitions(vme_backend_vulkan PRIVATE VK_NO_PROTOTYPES)