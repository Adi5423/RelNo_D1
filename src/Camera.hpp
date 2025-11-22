// Camera.hpp
// ---------------------------------------------------------
// A flexible first-person camera controller for OpenGL
// Features:
//  - Right mouse button + WASD for movement
//  - Mouse look (when right button held)
//  - Q/E for vertical movement
//  - Shift for speed boost
// ---------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float sprintMultiplier;
    float mouseSensitivity;
    float zoom;

    // Mouse state
    bool firstMouse;
    float lastX;
    float lastY;

    // Constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    ) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(2.5f),
        sprintMultiplier(2.0f),
        mouseSensitivity(0.1f),
        zoom(45.0f),
        firstMouse(true),
        lastX(400.0f),
        lastY(300.0f)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    // Returns the projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(zoom), aspectRatio, 0.1f, 100.0f);
    }

    // Process keyboard input
    void processKeyboard(GLFWwindow* window, float deltaTime) {
        // Only process movement when right mouse button is held
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
            return;
        }

        float velocity = movementSpeed * deltaTime;

        // Check for sprint (Shift key)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            velocity *= sprintMultiplier;
        }

        // WASD movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * velocity;

        // Q/E for vertical movement
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            position += worldUp * velocity;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            position -= worldUp * velocity;
    }

    // Process mouse movement
    void processMouseMovement(float xpos, float ypos, bool rightButtonPressed) {
        // Only update camera orientation when right mouse button is held
        if (!rightButtonPressed) {
            firstMouse = true;
            return;
        }

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constrain pitch to avoid screen flip
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCameraVectors();
    }

    // Process mouse scroll (zoom)
    void processMouseScroll(float yoffset) {
        zoom -= yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's Euler angles
    void updateCameraVectors() {
        // Calculate the new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Re-calculate the right and up vector
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};
