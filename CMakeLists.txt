cmake_minimum_required(VERSION 3.10)

project(RelNoTerrain3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------
# 1. ADD RELNO_D1 AS A SUBDIRECTORY
# ----------------------------------------------------------
# This will read relno_d1/CMakeLists.txt and build all
# the noise modules (WhiteNoise, PerlinNoise, etc.)
#
add_subdirectory(relno_d1)

# ----------------------------------------------------------
# 2. YOUR EXECUTABLE
# ----------------------------------------------------------
add_executable(RelNoTerrain3D
    src/main.cpp
)

# ----------------------------------------------------------
# 3. INCLUDE DIRECTORIES
# ----------------------------------------------------------
# RelNo_D1 installs headers here inside your local tree:
target_include_directories(RelNoTerrain3D
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/relno_d1/install/include
        ${CMAKE_CURRENT_SOURCE_DIR}/relno_d1
)

# ----------------------------------------------------------
# 4. LINK AGAINST RELNO_D1 NOISE LIBRARIES
# ----------------------------------------------------------
# These libraries exist inside:
# relno_d1/install/lib/libWhiteNoise.a
# relno_d1/install/lib/libPerlinNoise.a
# relno_d1/install/lib/libSimplexNoise.a
# relno_d1/install/lib/libPinkNoise.a

target_link_directories(RelNoTerrain3D PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/relno_d1/install/lib
)

target_link_libraries(RelNoTerrain3D
    PRIVATE WhiteNoise
    PRIVATE PerlinNoise
    PRIVATE SimplexNoise
    PRIVATE PinkNoise
)

# ----------------------------------------------------------
# 5. Warnings
# ----------------------------------------------------------
if (MSVC)
    target_compile_options(RelNoTerrain3D PRIVATE /W4)
else()
    target_compile_options(RelNoTerrain3D PRIVATE -Wall -Wextra -Wpedantic)
endif()
