add_library(engine_core STATIC
    src/types_engine_models.cpp
    src/lifecycle/engine_lifecycle_controller.cpp
)

target_include_directories(engine_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(engine_core PUBLIC cxx_std_20)

if(BUILD_TESTING)
    add_executable(engine_core_unit_observability_tests
        tests/unit/engine_lifecycle_observability_unit_tests.cpp
    )
    target_link_libraries(engine_core_unit_observability_tests PRIVATE engine_core)
    add_test(NAME engine_core.unit.observability COMMAND engine_core_unit_observability_tests)

    add_executable(engine_core_unit_types_contracts_tests
        tests/unit/engine_types_contracts_unit_tests.cpp
    )
    target_link_libraries(engine_core_unit_types_contracts_tests PRIVATE engine_core)
    add_test(NAME engine_core.unit.types_contracts COMMAND engine_core_unit_types_contracts_tests)

    add_executable(engine_core_integration_observability_enabled_tests
        tests/integration/engine_lifecycle_observability_enabled_integration.cpp
    )
    target_link_libraries(engine_core_integration_observability_enabled_tests PRIVATE engine_core)
    add_test(
        NAME engine_core.integration.observability_enabled
        COMMAND engine_core_integration_observability_enabled_tests
    )

    add_executable(engine_core_integration_observability_disabled_tests
        tests/integration/engine_lifecycle_observability_disabled_integration.cpp
    )
    target_link_libraries(engine_core_integration_observability_disabled_tests PRIVATE engine_core)
    add_test(
        NAME engine_core.integration.observability_disabled
        COMMAND engine_core_integration_observability_disabled_tests
    )

    add_executable(engine_core_integration_lifecycle_flow_tests
        tests/integration/engine_lifecycle_flow_integration.cpp
    )
    target_link_libraries(engine_core_integration_lifecycle_flow_tests PRIVATE engine_core)
    add_test(
        NAME engine_core.integration.lifecycle_flow
        COMMAND engine_core_integration_lifecycle_flow_tests
    )
endif()
