#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include "./include/Mesh.h"
// #include "./include/perlin.h"
// #include "./include/test.h"
#include "./include/Terrain.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
const unsigned int sceneN = 100;
const unsigned int sceneM = 100;

float lastFrame = glfwGetTime();
float deltaTime = 0;

Camera camera(SCR_WIDTH, SCR_HEIGHT);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    // Tell GLFW what version of OpenGL we are using (3.3 in this case)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So thats means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object  with a width, height and a name
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

    // Error checking
    if (window == NULL)
    {
        cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Callback for resize events
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Callback for scroll events
    glfwSetScrollCallback(window, scroll_callback);

    // load GLAD to get the pointers functions of OpenGL, respect to the current OS
    gladLoadGL();

    // Specify the viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Creates Shader object using shaders default.vert and default.frag
    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");

    Terrain plane(sceneM, sceneN);

    shaderProgram.Activate();
    float scaleFactor = 1.0f;
    shaderProgram.setFloat("scale", scaleFactor);

    // The ModelMatrix
    glm::mat4 boxModel = glm::mat4(1.0f);
    shaderProgram.setMat4("model", boxModel);

    //---------------Setting directional light in the scene---------------//
    shaderProgram.setVec3("dirLight.direction", 1.0f, -3.0f, 1.0f);
    // shaderProgram.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shaderProgram.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    shaderProgram.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    shaderProgram.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    // material settings
    shaderProgram.setFloat("material.shininess", 16.0f);
    //---------------Setting directional light in the scene---------------//

    glEnable(GL_DEPTH_TEST);

    // Precalculated Values to get a good point of view
    // Euler Angles: -35 -127
    float yawAngle = -127.0f, pitchAngle = -35.0f;
    glm::vec3 cameraPos = glm::vec3(12.4346f, 11.155f, 15.5763f);
    glm::vec3 cameraFront = glm::vec3(-0.492978, -0.573576, -0.654204);
    camera.setLocation(cameraPos, cameraFront, yawAngle, pitchAngle);

    float freq = plane.getFrequency(), amp = plane.getAmplitude();

    GLuint counter;
    float angle = 1.0f;
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Specify the color of the background
        glClearColor(0.525f, 0.796f, 0.976f, 1.0f);
        // rgb(134 203 249)

        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // FPS calculation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        counter++;
        if (deltaTime >= 1.0 / 30.0)
        {
            string FPS = to_string((1.0f / deltaTime) * counter);
            glfwSetWindowTitle(window, FPS.c_str());
            lastFrame = currentFrame;
            counter = 0;
        }
        camera.processInput(window, deltaTime);
        camera.updateMatrix();

        // Tells OpenGL which Shader Program we want to use
        // Activate the shader before exporting uniforms
        shaderProgram.Activate();

        // Exports the camera Position to the Fragment Shader for specular lighting
        shaderProgram.setVec3("camPos", camera.cameraPos);

        camera.getMatrix(shaderProgram, "camMatrix");

        // Draw the Terrain
        plane.checkUpdate();
        plane.drawTerrain(shaderProgram);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete all objects we've created
    shaderProgram.Delete();
    // Destroy Window object
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    // cout << glfwGetKey
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    camera.updateViewport(width, height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)(yoffset));
}
