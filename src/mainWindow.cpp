#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// Include RelNo_D1
#include "Noise.hpp"

using namespace Noise;

int main() 
{
    // -----------------------------
    // Init GLFW
    // -----------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, 
        "GameWindow + RelNo_D1", 
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // -----------------------------
    // Init GLAD
    // -----------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    // -----------------------------
    // Generate a Perlin Noise map
    // -----------------------------
    std::cout << "Generating Perlin noise using RelNo_D1...\n";

    auto noiseMap = create_perlinnoise(
        256, 256,
        40.0f,     // scale
        4,         // octaves
        1.0f,      // frequency
        0.5f,      // persistence
        2.0f,      // lacunarity
        0.0f,      // base offset
        21,        // seed
        OutputMode::None,
        ""
    );

    std::cout << "Noise generated! Sample value = "
              << noiseMap[100][100] << "\n";

    // -----------------------------
    // Main Loop (render simple bg)
    // -----------------------------
    while (!glfwWindowShouldClose(window)) 
    {
        glClearColor(0.22f, 0.28f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Later: upload the noise as a texture,
        // render full-screen quad => clouds.

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
