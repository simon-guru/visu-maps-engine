add_library(engine_render STATIC
    src/basic_frame_renderer.cpp
)

target_include_directories(engine_render
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine_render
    PUBLIC
        engine_gfx
)

target_compile_features(engine_render PUBLIC cxx_std_20)

add_executable(engine_render_basic_frame_integration_tests
    tests/integration/basic_frame_path_integration_tests.cpp
)

target_link_libraries(engine_render_basic_frame_integration_tests
    PRIVATE
        engine_render
)

target_compile_features(engine_render_basic_frame_integration_tests PRIVATE cxx_std_20)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME engine_render_basic_frame_integration_tests COMMAND engine_render_basic_frame_integration_tests)
endif()
