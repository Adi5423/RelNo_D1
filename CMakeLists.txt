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

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -------------------------------------------------------
# External dependencies: GLFW, GLAD, GLM, STB
# -------------------------------------------------------

add_subdirectory(vendor/glfw)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

include_directories(
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/stb
)

# -------------------------------------------------------
# GameWindow executable
# -------------------------------------------------------
add_executable(GameWindow
    src/mainWindow.cpp
)

target_link_libraries(GameWindow PRIVATE
    glad
    glfw
)
