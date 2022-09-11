#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"
#include "./glm/gtc/type_ptr.hpp"
#include "./glm/gtx/rotate_vector.hpp"
#include "./glm/gtx/vector_angle.hpp"

#include "shaderClass.h"

// Perspective
const float NEAR_PLANE = 0.1f; // nearPlane (PERSPECTIVE)
const float FAR_PLANE = 100.f; // farPlane (PERSPECTIVE)
const float FOV = 45.0f;       // field of view (degrees)

// Euler Angles (Roll is not used)
const float YAW = -90.0f;
const float PITCH = 0.0f;

// Offset
const float OFFSET = 10.0f;

// Default vectors
const float CAMERA_SPEED_DEFAULT = 2.5f; // cameraSpeed
// World
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
// Camera
const glm::vec3 CAMERA_FRONT_DEFAULT(0.0f, 0.0f, -1.0f);
const glm::vec3 CAMERA_UP_DEFAULT(0.0f, 1.0f, 0.0f);
const glm::vec3 CAMERA_POSITION_DEFAULT(0.0f, 0.0f, 3.0f);
const glm::vec3 CAMERA_TARGET_DEFAULT(0.0f, 0.0f, 0.0f);
const glm::mat4 CAMERA_MATRIX_DEFAULT(1.0f);

class Camera
{
private:
    glm::mat4 lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    glm::mat4 perspective(float FOVdeg, float aspectRatio, float nearPlane, float farPlane);
    void updateCameraVectors();
    void resetCameraPosition();
    // Methods
public:
    Camera(int width, int height);
    void setLocation(glm::vec3 pos, glm::vec3 front, float yawAngle, float pitchAngle);
    void getMatrix(Shader &shader, const char *uniform);
    void updateMatrix();
    void processInput(GLFWwindow *window, float deltaTime);
    void processMouseScroll(float yoffset);
    void updateDeltaTime(float deltaTime);
    void updateViewport(int width, int height);

    // Camera vectors
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;

    // Attributes
private:
    // Camera Matrix
    glm::mat4 cameraMatrix;

    // viewport
    int width;
    int height;

    // perspective projection
    float nearPlane;
    float farPlane;
    float fov;

    // Camera movement
    float cameraSpeed = CAMERA_SPEED_DEFAULT;
    float sensitivity = 0.1f;
    bool rotationMovement = false;
    float pitch, yaw;
    
    // Mouse Controls
    bool firstClick = true;
    float lastX = width / 2, lastY = height / 2;
};

#endif
