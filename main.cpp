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
// void generateTerrain(vector<vector<GLfloat>> &positions);
// glm::vec3 getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
const unsigned int sceneN = 100;
const unsigned int sceneM = 100;

float lastFrame = glfwGetTime();
float deltaTime = 0;

Camera camera(SCR_WIDTH, SCR_HEIGHT);

GLuint cubeIndices[] = {
    0, 1, 2, // first face
    2, 3, 0, // first face

    4, 5, 6, // second face
    6, 7, 4, // second face

    8, 9, 10,  // third face
    10, 11, 8, // third face

    12, 13, 14, // fourth face
    14, 15, 12, // fourth face

    16, 17, 18, // fith face
    18, 19, 16, // fith face

    20, 21, 22, // sixth face
    22, 23, 20, // sixth face

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

    Texture textures[]{
        // Texture
        Texture("./resources/Textures/grassTexture.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("./resources/Textures/grassTexture.png", "specular", 1, GL_RGBA, GL_UNSIGNED_BYTE),
    };

    // Creates Shader object using shaders default.vert and default.frag
    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");

    // vector<Vertex> cubeVerts(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(Vertex));
    // vector<GLuint> ind(cubeIndices, cubeIndices + sizeof(cubeIndices) / sizeof(GLuint));

    // int tamN = sceneN + 1, tamM = sceneM + 1;

    // int numVert = (2 * (tamN - 1)) * (2 * (tamM - 1));
    // vector<Vertex> verts(numVert);

    // // Vector to track the common vertices at one point (1,2)->{5,6,9,10}
    // vector<vector<vector<GLuint>>> commonVert(tamN, vector<vector<GLuint>>(tamM));
    // // Generate Vertex
    // for (int posz = 0, idx = 0; posz <= tamN - 1; posz++)
    // {
    //     float distance = 0.1f;
    //     int lastN = 2 * (tamN - 1);
    //     int lastM = 2 * (tamM - 1);
    //     for (int posx = 0; posx <= tamM - 1; posx++)
    //     {
    //         verts[idx].normal = glm::vec3(0.0f, 1.0f, 0.0f);
    //         verts[idx].color = glm::vec3(0.0f, 0.0f, 1.0f);
    //         verts[idx].position = glm::vec3(distance * posx, 1.0f, distance * posz);

    //         // The corners
    //         if ((posz == 0 && posx == 0) || (posz == 0 && posx == tamM - 1) || (posz == tamN - 1 && posx == 0) || (posx == tamM - 1 && posz == tamN - 1))
    //         {
    //             commonVert[posz][posx].push_back(idx);
    //             idx++;
    //         }
    //         // (0, X) ^ (tamN, X)
    //         else if ((posz == 0 && posx < tamM - 1) || (posz == tamN - 1 && posx < tamM - 1))
    //         {
    //             verts[idx + 1] = verts[idx];
    //             commonVert[posz][posx].push_back(idx);
    //             commonVert[posz][posx].push_back(idx + 1);
    //             idx += 2;
    //         }
    //         // (X, 0) ^ (X, tamM)
    //         else if ((posx == 0 && posz < tamN - 1) || (posx == tamN - 1 && posz < tamN - 1))
    //         {
    //             verts[idx + lastN] = verts[idx];
    //             commonVert[posz][posx].push_back(idx);
    //             commonVert[posz][posx].push_back(idx + lastN);

    //             // Si estas al final de la columna salta en lastN indices sino ve al siguiente
    //             idx += (posx == tamN - 1) ? lastN + 1 : 1;
    //         }
    //         // The middle points (X,X)
    //         else if (posx > 0 && posx < tamM - 1 && posz > 0 && posz < tamN - 1)
    //         {
    //             verts[idx + 1] = verts[idx + lastN] = verts[idx + lastN + 1] = verts[idx];
    //             commonVert[posz][posx].push_back(idx);
    //             commonVert[posz][posx].push_back(idx + 1);
    //             commonVert[posz][posx].push_back(idx + lastN);
    //             commonVert[posz][posx].push_back(idx + lastN + 1);
    //             idx += 2;
    //         }
    //     }
    // }

    // // // Generate Indices
    // int numInd = ((tamN - 1) * (tamM - 1)) * 6;
    // vector<GLuint> ind(numInd);
    // for (int row = 0, idx = 0; row < tamN - 1; row++)
    // {
    //     int lastN = 2 * (tamN - 1);
    //     for (int col = 0, val = 2 * lastN * (row); col < tamM - 1; col++, val += 2)
    //     {
    //         // First triangle indices
    //         ind[idx++] = val;
    //         ind[idx++] = val + 1;
    //         ind[idx++] = val + lastN;

    //         // Second Triangle indices
    //         ind[idx++] = val + 1;
    //         ind[idx++] = val + lastN;
    //         ind[idx++] = val + lastN + 1;
    //     }
    // }
    // // // cout << "Indices:\n";
    // // // for (auto i = 0u; i < ind.size(); i += 3)
    // // // cout << ind[i] << " " << ind[i + 1] << " " << ind[i + 2] << "\n";

    // // // for perlinNoise calculation
    // vector<vector<GLfloat>> terrainPositions(tamN, vector<GLfloat>(tamM));
    // generateTerrain(terrainPositions);

    // // // Assing the corresponding height to the Y component of the vertices
    // for (unsigned int posz = 0, idx = 0; posz < terrainPositions.size(); posz++)
    // {
    //     for (unsigned int posx = 0; posx < terrainPositions[0].size(); posx++)
    //     {
    //         float mapHeight = 7.5f, noise = terrainPositions[posz][posx];
    //         for (auto v : commonVert[posz][posx])
    //         {
    //             verts[v].position.y += noise * mapHeight * 0.4f;

    //             glm::vec3 color;
    //             float rgb = (noise);

    //             if (noise < 0.4) // Water
    //                 color = glm::vec3(0.0f, 0.0f, 2 * rgb);
    //             else if (noise < 0.7) // Terrain
    //                 color = glm::vec3(0.0f, rgb, rgb * 0.5f);
    //             else // Mountain
    //                 color = glm::vec3(rgb, rgb, rgb);
    //             verts[v].color = color;
    //         }
    //     }
    // }

    // // Assing the correct Normal Vector to each Face
    // for (int row = 0, idx = 0; row < tamN - 1; row++)
    // {
    //     int lastN = 2 * (tamN - 1);
    //     for (int col = 0, val = 2 * lastN * (row); col < tamM - 1; col++, val += 2)
    //     {

    //         glm::vec3 normal = getNormalVector(verts[val].position, verts[val + 1].position, verts[val + lastN].position);
    //         verts[val].normal = normal;
    //         verts[val + 1].normal = normal;
    //         verts[val + lastN].normal = normal;
    //         verts[val + lastN + 1].normal = normal;
    //     }
    // }

    // for (auto v : verts)
    // cout << v.normal.x << " " << v.normal.y << " " << v.normal.z << '\n';

    //---------------Setting objects in the scene---------------//
    // vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    // Mesh plane(verts, ind, tex);

    // cout << testE.value << "\n";

    Terrain plane(sceneM, sceneN);

    shaderProgram.Activate();
    float scaleFactor = 1.0f;
    shaderProgram.setFloat("scale", scaleFactor);

    // The ModelMatrix
    glm::mat4 boxModel = glm::mat4(1.0f);
    shaderProgram.setMat4("model", boxModel);
    // shaderProgram.setMat4(`"model", boxModel);

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

    // float angle = 30.0f; // degrees

    unsigned int counter = 0;
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Specify the color of the background
        glClearColor(0.525f, 0.796f, 0.976f, 1.0f);
        // rgb(134 203 249)

        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

// glm::vec3 getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3)
// {
//     glm::vec3 firstV = vert2 - vert1;
//     glm::vec3 secondV = vert3 - vert1;

//     return glm::normalize(glm::cross(firstV, secondV));
// }

// void generateTerrain(vector<vector<GLfloat>> &positions)
// {
//     srand(time(0));
//     setUp();
//     GLuint layers = 5;
//     float maxNoiseValue = 0;
//     vector<GLfloat> Noises;
//     for (int i = 0; i < positions.size(); i++)
//     {
//         for (int j = 0; j < positions[0].size(); j++)
//         {
//             float amplitude = 1.0f, frequency = 0.04f, total = 0;
//             for (int k = 0; k < layers; k++)
//             {
//                 float noise = amplitude * 0.5f * (perlinNoise(i * frequency, j * frequency) + 1.0f);
//                 total += noise;
//                 frequency *= 2;
//                 amplitude *= 0.5;
//             }
//             maxNoiseValue = max(maxNoiseValue, total);
//             Noises.push_back(total);
//         }
//     }
//     for (float &n : Noises)
//         n /= maxNoiseValue;

//     for (int i = 0, count = 0; i < positions.size(); i++)
//     {
//         for (int j = 0; j < positions[0].size(); j++)
//         {
//             positions[i][j] = Noises[count++];
//         }
//     }
// }
