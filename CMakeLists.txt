cmake_minimum_required(VERSION 3.10)
project(RelNo_D1 VERSION 0.1.0 LANGUAGES CXX)

# Allow user to disable building examples
option(BUILD_EXAMPLES "Build example executable" ON)

# Quiet MSVC "unsafe" warnings from stb
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Make project root headers findable (Noise.hpp lives at project root)
include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add submodules
add_subdirectory(NoiseMaps)

# Example app (optional)
if (BUILD_EXAMPLES)
    add_executable(RelNoD_NoiseExample examples/main.cpp)
    target_link_libraries(RelNoD_NoiseExample PRIVATE WhiteNoise PerlinNoise SimplexNoise)
    target_compile_definitions(RelNoD_NoiseExample PRIVATE "RelNo_D1_EXAMPLE")
endif()

# Installation targets (install all libs and headers)
install(TARGETS WhiteNoise PerlinNoise SimplexNoise
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY NoiseMaps/WhiteNoise/include/ DESTINATION include/Noise/WhiteNoise)
install(DIRECTORY NoiseMaps/PerlinNoise/include/ DESTINATION include/Noise/PerlinNoise)
install(DIRECTORY NoiseMaps/SimplexNoise/include/ DESTINATION include/Noise/SimplexNoise)
install(FILES Noise.hpp DESTINATION include/Noise)

