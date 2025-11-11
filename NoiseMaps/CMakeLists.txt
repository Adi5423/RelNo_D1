# NoiseMaps/CMakeLists.txt
# Builds each noise module as a static library and provides a single
# small OBJECT library for stb implementation.

# WhiteNoise library
add_library(WhiteNoise STATIC
    WhiteNoise/src/WhiteNoise.cpp
)
target_include_directories(WhiteNoise PUBLIC
    WhiteNoise/include
    ${CMAKE_SOURCE_DIR}/external
)
target_link_libraries(WhiteNoise PRIVATE STBImageWrite)

# PerlinNoise library
add_library(PerlinNoise STATIC
    PerlinNoise/src/PerlinNoise.cpp
)
target_include_directories(PerlinNoise PUBLIC
    PerlinNoise/include
    ${CMAKE_SOURCE_DIR}/external
)
target_link_libraries(PerlinNoise PRIVATE STBImageWrite)

# SimplexNoise library
add_library(SimplexNoise STATIC
    SimplexNoise/src/SimplexNoise.cpp
)
target_include_directories(SimplexNoise PUBLIC
    SimplexNoise/include
    ${CMAKE_SOURCE_DIR}/external
)
target_link_libraries(SimplexNoise PRIVATE STBImageWrite)

# Shared stb implementation compiled once (OBJECT => no duplicate symbols)
add_library(STBImageWrite OBJECT ${CMAKE_SOURCE_DIR}/external/stb_impl.cpp)

# (Note: STBImageWrite is linked into each module above)
