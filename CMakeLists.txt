# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 2026-03-31
# This file is part of VISU LLC.
# <simon.o2000dev@gmail.com>
# github.com/o2000dev
cmake_minimum_required(VERSION 3.31)
project(visu-maps-engine LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_subdirectory(engine)
