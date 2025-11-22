#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// Include GLM for camera math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include Camera controller
#include "Camera.hpp"

// Include RelNo_D1
#include "Noise.hpp"

using namespace Noise;

// -----------------------------
// Global state
// -----------------------------
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rightMousePressed = false;

// -----------------------------
// Callbacks
// -----------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos), rightMousePressed);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMousePressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE) {
            rightMousePressed = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement (handled in Camera class)
    camera.processKeyboard(window, deltaTime);
}

// -----------------------------
// Main
// -----------------------------
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
        "GameWindow - Camera Controller", 
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // -----------------------------
    // Init GLAD
    // -----------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "\n=== Camera Controls ===\n";
    std::cout << "Hold RIGHT MOUSE BUTTON to activate camera\n";
    std::cout << "  WASD       - Move forward/left/back/right\n";
    std::cout << "  Q/E        - Move up/down\n";
    std::cout << "  SHIFT      - Sprint (2x speed)\n";
    std::cout << "  Mouse Move - Look around\n";
    std::cout << "  ESC        - Exit\n";
    std::cout << "=======================\n\n";

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
    // Main Loop
    // -----------------------------
    std::cout << "\nWindow opened. Try the camera controls!\n";

    while (!glfwWindowShouldClose(window)) 
    {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Render
        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Render your 3D scene here using camera.getViewMatrix() and camera.getProjectionMatrix()
        // Example:
        // glm::mat4 view = camera.getViewMatrix();
        // glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
