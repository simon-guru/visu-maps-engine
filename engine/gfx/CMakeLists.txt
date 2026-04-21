add_library(engine_gfx STATIC
    src/commands/simple_command_encoder.cpp
    src/contracts/stub_gfx_queue.cpp
    src/contracts/stub_gfx_platform.cpp
    src/resources/stub_resources.cpp
)

target_include_directories(engine_gfx
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(engine_gfx PUBLIC cxx_std_20)

add_executable(engine_gfx_commands_unit_tests
    tests/unit/command_encoder_contract_tests.cpp
)

target_link_libraries(engine_gfx_commands_unit_tests
    PRIVATE
        engine_gfx
)

target_compile_features(engine_gfx_commands_unit_tests PRIVATE cxx_std_20)


add_executable(engine_gfx_pipeline_unit_tests
    tests/unit/pipeline_contract_tests.cpp
)

target_link_libraries(engine_gfx_pipeline_unit_tests
    PRIVATE
        engine_gfx
)

target_compile_features(engine_gfx_pipeline_unit_tests PRIVATE cxx_std_20)


add_executable(engine_gfx_resources_unit_tests
    tests/unit/resources_contract_tests.cpp
)

target_link_libraries(engine_gfx_resources_unit_tests
    PRIVATE
        engine_gfx
)

target_compile_features(engine_gfx_resources_unit_tests PRIVATE cxx_std_20)

add_executable(engine_gfx_queue_integration_tests
    tests/integration/queue_submit_integration_tests.cpp
)

target_link_libraries(engine_gfx_queue_integration_tests
    PRIVATE
        engine_gfx
)

target_compile_features(engine_gfx_queue_integration_tests PRIVATE cxx_std_20)

add_executable(engine_gfx_frame_integration_tests
    tests/integration/frame_basic_integration_tests.cpp
)

target_link_libraries(engine_gfx_frame_integration_tests
    PRIVATE
        engine_gfx
)

target_compile_features(engine_gfx_frame_integration_tests PRIVATE cxx_std_20)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME engine_gfx_commands_unit_tests COMMAND engine_gfx_commands_unit_tests)
    add_test(NAME engine_gfx_pipeline_unit_tests COMMAND engine_gfx_pipeline_unit_tests)
    add_test(NAME engine_gfx_resources_unit_tests COMMAND engine_gfx_resources_unit_tests)
    add_test(NAME engine_gfx_queue_integration_tests COMMAND engine_gfx_queue_integration_tests)
    add_test(NAME engine_gfx_frame_integration_tests COMMAND engine_gfx_frame_integration_tests)
endif()
