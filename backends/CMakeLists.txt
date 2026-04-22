# backends/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Opção para selecionar o backend (padrão: stub para testes)
set(VME_GFX_BACKEND "stub" CACHE STRING "Graphics backend to use (stub, vulkan, gl46, metal, gles, webgl)")

# Mapeia a opção para o diretório correspondente
set(BACKEND_DIR "")
if(VME_GFX_BACKEND STREQUAL "vulkan")
    set(BACKEND_DIR vulkan)
elseif(VME_GFX_BACKEND STREQUAL "gl46")
    set(BACKEND_DIR gl46)
elseif(VME_GFX_BACKEND STREQUAL "metal")
    set(BACKEND_DIR metal)
elseif(VME_GFX_BACKEND STREQUAL "gles")
    set(BACKEND_DIR gles)
elseif(VME_GFX_BACKEND STREQUAL "webgl")
    set(BACKEND_DIR webgl)
else()
    message(STATUS "No real backend selected, using stub backend from engine/gfx")
    # Não adiciona nenhum subdiretório, pois os stubs já estão em engine/gfx
endif()

if(BACKEND_DIR)
    add_subdirectory(${BACKEND_DIR})
endif()