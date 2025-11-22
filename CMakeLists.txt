cmake_minimum_required(VERSION 3.20)
project(GameWindow VERSION 0.1 LANGUAGES C CXX)

# -------------------------------------------------------
# Global C++ settings
# -------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# -------------------------------------------------------
# Colored output for Ninja/Make
# -------------------------------------------------------
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if(${FORCE_COLORED_OUTPUT})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

# -------------------------------------------------------
# Warning configuration
# -------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Strict warnings for our own code
    set(MY_COMPILE_OPTIONS -Wall -Wextra)
    
    # Complete warning suppression for vendor libraries
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")  # Suppress all C warnings (vendor code)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")  # Suppress all C++ warnings (vendor code)
endif()

# -------------------------------------------------------
# External dependencies: GLFW, GLAD, GLM, STB
# -------------------------------------------------------

# GLFW - Build quietly
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

# GLAD - OpenGL loader
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# GLM and STB - Header-only libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/stb
)

# -------------------------------------------------------
# RelNo_D1: Noise generation library
# -------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build RelNo_D1 examples" FORCE)
add_subdirectory(vendor/relno_d1)

# -------------------------------------------------------
# GameWindow executable - Our main application
# -------------------------------------------------------
add_executable(GameWindow
    src/mainWindow.cpp
)

target_include_directories(GameWindow PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/relno_d1
)

target_link_libraries(GameWindow PRIVATE
    glad
    glfw
    WhiteNoise
    PerlinNoise
    SimplexNoise
    PinkNoise
)

# Apply strict warnings ONLY to our GameWindow target
target_compile_options(GameWindow PRIVATE ${MY_COMPILE_OPTIONS})
