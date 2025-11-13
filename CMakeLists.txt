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
    target_link_libraries(RelNoD_NoiseExample PRIVATE WhiteNoise PerlinNoise SimplexNoise TerrainNoise)
    target_compile_definitions(RelNoD_NoiseExample PRIVATE "RelNo_D1_EXAMPLE")
    
    # Sampling API test
    add_executable(RelNoD_SamplingTest examples/test_sampling.cpp)
    target_link_libraries(RelNoD_SamplingTest PRIVATE WhiteNoise PerlinNoise SimplexNoise)
    target_compile_definitions(RelNoD_SamplingTest PRIVATE "RelNo_D1_SAMPLING_TEST")
    
    # Terrain generation test
    add_executable(RelNoD_TerrainTest examples/test_terrain.cpp)
    target_link_libraries(RelNoD_TerrainTest PRIVATE TerrainNoise PerlinNoise)
    target_compile_definitions(RelNoD_TerrainTest PRIVATE "RelNo_D1_TERRAIN_TEST")
    
    # Chunk generation test
    add_executable(RelNoD_ChunkTest examples/test_chunks.cpp)
    target_link_libraries(RelNoD_ChunkTest PRIVATE WhiteNoise PerlinNoise SimplexNoise TerrainNoise)
    target_compile_definitions(RelNoD_ChunkTest PRIVATE "RelNo_D1_CHUNK_TEST")
    
    # Cave generation test
    add_executable(RelNoD_CaveTest examples/test_caves.cpp)
    target_link_libraries(RelNoD_CaveTest PRIVATE CaveNoise PerlinNoise)
    target_compile_definitions(RelNoD_CaveTest PRIVATE "RelNo_D1_CAVE_TEST")
    
    # Tilemap export test
    add_executable(RelNoD_TilemapTest examples/test_tilemap.cpp)
    target_link_libraries(RelNoD_TilemapTest PRIVATE TilemapExport CaveNoise PerlinNoise TerrainNoise)
    target_compile_definitions(RelNoD_TilemapTest PRIVATE "RelNo_D1_TILEMAP_TEST")
endif()

# Installation setup — works on all platforms & paths
install(TARGETS WhiteNoise PerlinNoise SimplexNoise TerrainNoise CaveNoise TilemapExport
    EXPORT PackageNoiseTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Header installation
install(DIRECTORY NoiseMaps/WhiteNoise/include/ DESTINATION include/Noise/WhiteNoise)
install(DIRECTORY NoiseMaps/PerlinNoise/include/ DESTINATION include/Noise/PerlinNoise)
install(DIRECTORY NoiseMaps/SimplexNoise/include/ DESTINATION include/Noise/SimplexNoise)
install(DIRECTORY NoiseMaps/TerrainNoise/include/ DESTINATION include/Noise/TerrainNoise)
install(DIRECTORY NoiseMaps/CaveNoise/include/ DESTINATION include/Noise/CaveNoise)
install(DIRECTORY NoiseMaps/TilemapExport/include/ DESTINATION include/Noise/TilemapExport)
install(FILES Noise.hpp DESTINATION include/Noise)
