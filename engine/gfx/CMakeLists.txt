add_library(engine_gfx STATIC
    src/commands/simple_command_encoder.cpp
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
