add_library(engine_tiles STATIC
    src/render_snapshot_cache.cpp
)

target_include_directories(engine_tiles
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(engine_tiles PUBLIC cxx_std_20)
