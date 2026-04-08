add_library(engine_core STATIC
    src/types_engine_models.cpp
    src/lifecycle/engine_lifecycle_controller.cpp
)

target_include_directories(engine_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(engine_core PUBLIC cxx_std_20)
