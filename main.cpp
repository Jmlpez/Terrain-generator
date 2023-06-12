#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"
#include "./include/Terrain.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
const unsigned int sceneN = 150;
const unsigned int sceneM = 150;

float lastFrame = glfwGetTime();
float deltaTime = 0;

Camera camera(SCR_WIDTH, SCR_HEIGHT);

struct DirLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

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
    DirLight dirLight;
    dirLight.direction = glm::vec3(-0.2f, 0.2f, 0.6f);
    dirLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    dirLight.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    shaderProgram.setVec3("dirLight.direction", dirLight.direction);
    shaderProgram.setVec3("dirLight.ambient", dirLight.ambient);
    shaderProgram.setVec3("dirLight.diffuse", dirLight.diffuse);
    shaderProgram.setVec3("dirLight.specular", dirLight.specular);

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool drawTerrain = true;
    GLuint counter;
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

        // New Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Tells OpenGL which Shader Program we want to use
        // Activate the shader before exporting uniforms
        shaderProgram.Activate();

        // Exports the camera Position to the Fragment Shader for specular lighting
        shaderProgram.setVec3("camPos", camera.cameraPos);

        camera.getMatrix(shaderProgram, "camMatrix");

        // ImGui::Begin("My name is window, ImGui window");
        // ImGui::Text("Hi Mom!");
        // ImGui::Checkbox("Draw the shape", &drawTerrain);
        // ImGui::End();

        ImGui::Begin("Light");
        ImGui::ColorEdit3("Light Color", &dirLight.specular.x);
        ImGui::SliderFloat3("Light Position", &dirLight.direction.x, -1.0f, 1.f);
        ImGui::End();

        shaderProgram.setVec3("dirLight.direction", dirLight.direction);
        shaderProgram.setVec3("dirLight.specular", dirLight.specular);

        ImGui::Begin("Terrain Options");
        ImGui::SliderInt("Layers", &plane.layers, 1, 8);
        ImGui::SliderFloat("Frequency", &plane.frequency, 1.0f, 10.0f);
        ImGui::SliderFloat("Persistance", &plane.persistance, 0.1f, 1.0f);
        ImGui::SliderFloat("Lacunarity", &plane.lacunarity, 1.0f, 3.0f);
        ImGui::SliderFloat("Map Height", &plane.mapHeight, 0.0f, 15.0f);
        ImGui::InputFloat("Distance", &plane.distance, 0.01f);
        ImGui::InputInt("Dimension", &plane.dimension, 1);

        if (ImGui::Button("Reset Seed", ImVec2(100, 30)))
        {
            plane.resetSeed();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Draw the Terrain
        if (drawTerrain)
        {
            plane.checkUpdate();
            plane.drawTerrain(shaderProgram);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Delete ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
