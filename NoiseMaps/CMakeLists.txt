# NoiseMaps/CMakeLists.txt
# --------------------------------------------------
# Builds the individual noise libraries
# --------------------------------------------------

add_library(WhiteNoise STATIC
    WhiteNoise/src/WhiteNoise.cpp
)
target_include_directories(WhiteNoise PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/WhiteNoise/include
    ${CMAKE_SOURCE_DIR}/external
)

add_library(PerlinNoise STATIC
    PerlinNoise/src/PerlinNoise.cpp
)
target_include_directories(PerlinNoise PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/PerlinNoise/include
    ${CMAKE_SOURCE_DIR}/external
)

add_library(SimplexNoise STATIC
    SimplexNoise/src/SimplexNoise.cpp
)
target_include_directories(SimplexNoise PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include
    ${CMAKE_SOURCE_DIR}/external
)

add_library(TerrainNoise STATIC
    TerrainNoise/src/TerrainNoise.cpp
)
target_include_directories(TerrainNoise PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/TerrainNoise/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PerlinNoise/include
    ${CMAKE_SOURCE_DIR}/external
)

add_library(CaveNoise STATIC
    CaveNoise/src/CaveNoise.cpp
)
target_include_directories(CaveNoise PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CaveNoise/include
    ${CMAKE_CURRENT_SOURCE_DIR}/PerlinNoise/include
    ${CMAKE_SOURCE_DIR}/external
)

# Shared stb implementation (compiled once)
add_library(STBImageWrite OBJECT ${CMAKE_SOURCE_DIR}/external/stb_impl.cpp)

# Link stb to each noise library
target_link_libraries(WhiteNoise PRIVATE STBImageWrite)
target_link_libraries(PerlinNoise PRIVATE STBImageWrite)
target_link_libraries(SimplexNoise PRIVATE STBImageWrite)
target_link_libraries(TerrainNoise PRIVATE STBImageWrite PerlinNoise)
target_link_libraries(CaveNoise PRIVATE STBImageWrite PerlinNoise)
