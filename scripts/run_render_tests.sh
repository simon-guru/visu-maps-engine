#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${OUT_DIR:-/tmp/vme-render-checks}"
mkdir -p "${OUT_DIR}"

CXX="${CXX:-g++}"
CXXFLAGS=(
    -std=c++20
    -I"${ROOT_DIR}/engine/render/include"
    -I"${ROOT_DIR}/engine/gfx/include"
    -I"${ROOT_DIR}/engine/tiles/include"
    -I"${ROOT_DIR}/engine/services/include"
    -I"${ROOT_DIR}/engine/core/include"   # necessário para EngineError, etc.
)

# Fontes dos módulos dependentes
GFX_SOURCES=(
    "${ROOT_DIR}/engine/gfx/src/commands/simple_command_encoder.cpp"
    "${ROOT_DIR}/engine/gfx/src/contracts/stub_gfx_queue.cpp"
    "${ROOT_DIR}/engine/gfx/src/contracts/stub_gfx_platform.cpp"
    "${ROOT_DIR}/engine/gfx/src/resources/backend_translation.cpp"
    "${ROOT_DIR}/engine/gfx/src/resources/stub_resources.cpp"
)

TILES_SOURCES=(
    "${ROOT_DIR}/engine/tiles/src/render_snapshot_cache.cpp"
)

RENDER_SOURCES=(
    "${ROOT_DIR}/engine/render/src/basic_frame_renderer.cpp"
    "${ROOT_DIR}/engine/render/src/frame_plan.cpp"
    "${ROOT_DIR}/engine/render/src/material_pipeline_cache.cpp"
    "${ROOT_DIR}/engine/render/src/scene_data_adapter.cpp"
)

echo "[1/2] Building and running render unit tests..."
"${CXX}" "${CXXFLAGS[@]}" \
    "${GFX_SOURCES[@]}" \
    "${TILES_SOURCES[@]}" \
    "${RENDER_SOURCES[@]}" \
    "${ROOT_DIR}/engine/render/tests/unit/render_frame_plan_unit_tests.cpp" \
    -o "${OUT_DIR}/render_unit"
"${OUT_DIR}/render_unit"

echo "[2/2] Building and running render integration tests..."
"${CXX}" "${CXXFLAGS[@]}" \
    "${GFX_SOURCES[@]}" \
    "${TILES_SOURCES[@]}" \
    "${RENDER_SOURCES[@]}" \
    "${ROOT_DIR}/engine/render/tests/integration/basic_frame_path_integration_tests.cpp" \
    -o "${OUT_DIR}/render_integration"
"${OUT_DIR}/render_integration"

echo "All engine/render tests passed."