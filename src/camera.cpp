#include "../include/camera.h"

// Left Handed Coordinate System
Camera::Camera(int width, int height)
{
    // Viewport Width and Height
    this->width = width;
    this->height = height;

    // Camera Vectors
    cameraPos = CAMERA_POSITION_DEFAULT;
    cameraFront = CAMERA_FRONT_DEFAULT;
    cameraRight = glm::normalize(glm::cross(WORLD_UP, cameraFront));
    cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

    // this->lookAt(glm::vec3(0.0f, 0.0f, 3.0f), )

    // Camera matrix
    cameraMatrix = CAMERA_MATRIX_DEFAULT;

    // Perspective projection
    nearPlane = NEAR_PLANE;
    farPlane = FAR_PLANE;
    fov = FOV;

    // Euler Angles
    pitch = PITCH;
    yaw = YAW;
}

// Right Handed Coordinate System
glm::mat4 Camera::lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{

    // Get the UVN vectors by performing cross products (U: X, V: Y, N: Z)
    glm::vec3 frontV = glm::normalize(target - position);
    glm::vec3 rightV = glm::normalize(glm::cross(frontV, up));
    glm::vec3 upV = glm::normalize(glm::cross(rightV, frontV));

    // It's necessary flip the front vector because we pointing to the negative z-axis (Right-Handed Coordinate system)
    frontV = -frontV;

    glm::mat4 matrix(rightV.x, rightV.y, rightV.z, -glm::dot(rightV, position),
                     upV.x, upV.y, upV.z, -glm::dot(upV, position),
                     frontV.x, frontV.y, frontV.z, -glm::dot(frontV, position),
                     0.0f, 0.0f, 0.0f, 1.0f);

    // Because glm is column-major
    matrix = glm::transpose(matrix);
    // camera-matrix (view matrix, eye-space)
    return matrix;
}

glm::mat4 Camera::perspective(float FOVdeg, float aspectRatio, float nearPlane, float farPlane)
{

    float tanHalf = tanf(glm::radians(FOVdeg / 2.0f));
    float d = 1.0f / tanHalf;

    // float aspectRatio = this->width / this->height;

    float zRange = nearPlane - farPlane;
    float A = (-farPlane - nearPlane) / zRange;
    float B = 2 * farPlane * nearPlane / zRange;

    // projection Matrix
    glm::mat4 projection = glm::mat4(d / aspectRatio, 0.0f, 0.0f, 0.0f,
                                     0.0f, d, 0.0f, 0.0f,
                                     0.0f, 0.0f, A, B,
                                     0.0f, 0.0f, 1.0f, 0.0f);

    projection = glm::transpose(projection);
    return projection;
}

void Camera::setLocation(glm::vec3 pos, glm::vec3 front, float yawAngle, float pitchAngle)
{
    // Camera Vectors
    cameraPos = pos;
    cameraFront = front;
    cameraRight = glm::normalize(glm::cross(cameraFront, WORLD_UP));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    // Camera matrix
    cameraMatrix = CAMERA_MATRIX_DEFAULT;

    // Euler Angles
    pitch = pitchAngle;
    yaw = yawAngle;

    updateMatrix();
}

void Camera::resetCameraPosition()
{
    // Camera Vectors
    cameraPos = CAMERA_POSITION_DEFAULT;
    cameraFront = CAMERA_FRONT_DEFAULT;
    cameraRight = glm::normalize(glm::cross(cameraFront, WORLD_UP));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    // Camera matrix
    cameraMatrix = CAMERA_MATRIX_DEFAULT;

    // Euler Angles
    pitch = PITCH;
    yaw = YAW;

    updateMatrix();
}

void Camera::updateMatrix()
{
    glm::mat4 view = this->lookAt(cameraPos, cameraPos + cameraFront, WORLD_UP);
    // glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, WORLD_UP);
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / height, nearPlane, farPlane);
    cameraMatrix = projection * view;
}

void Camera::updateViewport(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Camera::getMatrix(Shader &shader, const char *uniform)
{
    // Exports camera matrix
    shader.setMat4(uniform, cameraMatrix);
}

void Camera::processInput(GLFWwindow *window, float deltaTime)
{
    rotationMovement = false;
    this->updateDeltaTime(deltaTime);
    // Moving forwards
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    // Moving left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    // Moving backwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    // Moving right (invert the cross product order)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    // Moving up
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraUp;
    }
    // Moving Down
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraSpeed = CAMERA_SPEED_DEFAULT * 1.5;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        cameraSpeed = CAMERA_SPEED_DEFAULT;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        rotationMovement = true;
        yaw += OFFSET * sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        rotationMovement = true;
        yaw -= OFFSET * sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        rotationMovement = true;
        pitch += OFFSET * sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        rotationMovement = true;
        pitch += -OFFSET * sensitivity;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        resetCameraPosition();
    }
    /*
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (firstClick)
        {
            lastX = mouseX;
            lastY = mouseY;
            firstClick = false;
        }

        float xoffset = mouseX - lastX;
        float yoffset = lastY - mouseY;
        lastX = mouseX;
        lastY = mouseY;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // prevent from flip the camera horizontally
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCameraVectors();
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
    */
    if (rotationMovement)
    {
        // prevent from flip the camera horizontally
        if (pitch >= 88.0f)
            pitch = 88.0f;
        if (pitch < -88.0f)
            pitch = -88.0f;
        updateCameraVectors();
    }
}

void Camera::updateCameraVectors()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    cameraRight = glm::normalize(glm::cross(cameraFront, WORLD_UP));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::updateDeltaTime(float deltaTime)
{
    if (deltaTime != 0)
        cameraSpeed = CAMERA_SPEED_DEFAULT * deltaTime;
}

void Camera::processMouseScroll(float yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}