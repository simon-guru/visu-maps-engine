#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${OUT_DIR:-/tmp/vme-checks}"
ITERATIONS="${1:-200000}"

mkdir -p "${OUT_DIR}"

CXX="${CXX:-g++}"
CXXFLAGS=(
  -std=c++20
  -fpermissive
  -I"${ROOT_DIR}/engine/core/include"
)
CORE_SOURCES=(
  "${ROOT_DIR}/engine/core/src/types_engine_models.cpp"
  "${ROOT_DIR}/engine/core/src/lifecycle/engine_lifecycle_controller.cpp"
)

echo "[1/4] Building and running unit tests..."
"${CXX}" "${CXXFLAGS[@]}" \
  "${CORE_SOURCES[@]}" \
  "${ROOT_DIR}/engine/core/tests/unit/engine_lifecycle_observability_unit_tests.cpp" \
  -o "${OUT_DIR}/unit_obs_test"
"${OUT_DIR}/unit_obs_test"

echo "[2/4] Building and running integration tests (observability enabled)..."
"${CXX}" "${CXXFLAGS[@]}" \
  "${CORE_SOURCES[@]}" \
  "${ROOT_DIR}/engine/core/tests/integration/engine_lifecycle_observability_enabled_integration.cpp" \
  -o "${OUT_DIR}/int_obs_on"
"${OUT_DIR}/int_obs_on"

echo "[3/4] Building and running integration tests (observability disabled)..."
"${CXX}" "${CXXFLAGS[@]}" \
  "${CORE_SOURCES[@]}" \
  "${ROOT_DIR}/engine/core/tests/integration/engine_lifecycle_observability_disabled_integration.cpp" \
  -o "${OUT_DIR}/int_obs_off"
"${OUT_DIR}/int_obs_off"

echo "[4/4] Building and running tick() microbenchmark (iterations=${ITERATIONS})..."
"${CXX}" "${CXXFLAGS[@]}" -O2 \
  "${CORE_SOURCES[@]}" \
  "${ROOT_DIR}/tests/performance/tick_observability_benchmark.cpp" \
  -o "${OUT_DIR}/tick_bench"
"${OUT_DIR}/tick_bench" "${ITERATIONS}"

echo "All observability checks passed."
