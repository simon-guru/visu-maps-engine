#!/usr/bin/env bash
set -euo pipefail

# --- Configuração de diretórios ---
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${OUT_DIR:-/tmp/vme-gfx-checks}"
mkdir -p "${OUT_DIR}"

# --- Compilador e flags ---
CXX="${CXX:-g++}"
CXXFLAGS=(
    -std=c++20
    -I"${ROOT_DIR}/engine/gfx/include"
    -I"${ROOT_DIR}/engine/core/include"   # necessário para contratos comuns (EngineError, etc.)? O gfx é independente do core? Verificar dependências.
)

# Fontes do módulo gfx (conforme CMakeLists.txt)
GFX_SOURCES=(
    "${ROOT_DIR}/engine/gfx/src/commands/simple_command_encoder.cpp"
    "${ROOT_DIR}/engine/gfx/src/contracts/stub_gfx_queue.cpp"
    "${ROOT_DIR}/engine/gfx/src/contracts/stub_gfx_platform.cpp"
    "${ROOT_DIR}/engine/gfx/src/resources/backend_translation.cpp"
    "${ROOT_DIR}/engine/gfx/src/resources/stub_resources.cpp"
)

# --- Função auxiliar para compilar e executar um teste ---
run_test() {
    local test_name="$1"
    local test_source="$2"
    local extra_sources=("${@:3}")

    echo "Building ${test_name}..."
    "${CXX}" "${CXXFLAGS[@]}" \
        "${GFX_SOURCES[@]}" \
        "${test_source}" \
        "${extra_sources[@]}" \
        -o "${OUT_DIR}/${test_name}"

    echo "Running ${test_name}..."
    "${OUT_DIR}/${test_name}"
}

# --- Execução dos testes ---

echo "[1/5] Building and running commands unit tests..."
run_test "gfx_commands_unit" \
    "${ROOT_DIR}/engine/gfx/tests/unit/command_encoder_contract_tests.cpp"

echo "[2/5] Building and running pipeline unit tests..."
run_test "gfx_pipeline_unit" \
    "${ROOT_DIR}/engine/gfx/tests/unit/pipeline_contract_tests.cpp"

echo "[3/5] Building and running resources unit tests..."
run_test "gfx_resources_unit" \
    "${ROOT_DIR}/engine/gfx/tests/unit/resources_contract_tests.cpp"

echo "[4/5] Building and running queue integration tests..."
run_test "gfx_queue_integration" \
    "${ROOT_DIR}/engine/gfx/tests/integration/queue_submit_integration_tests.cpp"

echo "[5/5] Building and running frame integration tests..."
run_test "gfx_frame_integration" \
    "${ROOT_DIR}/engine/gfx/tests/integration/frame_basic_integration_tests.cpp"

echo "All engine/gfx tests passed."