# NoiseMaps/CMakeLists.txt
# --------------------------------------------------
# Builds each noise library with correct install/export support
# --------------------------------------------------

# Shared stb implementation
add_library(STBImageWrite OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/stb_impl.cpp
)

# --------------------------------------------------
# WhiteNoise
# --------------------------------------------------
add_library(WhiteNoise STATIC
    WhiteNoise/src/WhiteNoise.cpp
)

target_include_directories(WhiteNoise PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/WhiteNoise/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> 
    $<INSTALL_INTERFACE:include/Noise/WhiteNoise>
    $<INSTALL_INTERFACE:include/Noise>
)

target_link_libraries(WhiteNoise PRIVATE STBImageWrite)

# --------------------------------------------------
# PerlinNoise
# --------------------------------------------------
add_library(PerlinNoise STATIC
    PerlinNoise/src/PerlinNoise.cpp
)

target_include_directories(PerlinNoise PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PerlinNoise/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> 
    $<INSTALL_INTERFACE:include/Noise/PerlinNoise>
    $<INSTALL_INTERFACE:include/Noise>
)

target_link_libraries(PerlinNoise PRIVATE STBImageWrite)

# --------------------------------------------------
# SimplexNoise
# --------------------------------------------------
add_library(SimplexNoise STATIC
    SimplexNoise/src/SimplexNoise.cpp
)

target_include_directories(SimplexNoise PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SimplexNoise/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>   
    $<INSTALL_INTERFACE:include/Noise/SimplexNoise>
    $<INSTALL_INTERFACE:include/Noise>
)

target_link_libraries(SimplexNoise PRIVATE STBImageWrite)

# --------------------------------------------------
# PinkNoise
# --------------------------------------------------
add_library(PinkNoise STATIC
    PinkNoise/src/PinkNoise.cpp
)

target_include_directories(PinkNoise PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PinkNoise/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> 
    $<INSTALL_INTERFACE:include/Noise/PinkNoise>
    $<INSTALL_INTERFACE:include/Noise>
)

target_link_libraries(PinkNoise PRIVATE STBImageWrite)

