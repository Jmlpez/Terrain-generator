#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include "./glm/glm.hpp"
#include <glad/glad.h>
#include <vector>

using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoords;
};

class VBO
{
public:
    GLuint ID;
    VBO(vector<Vertex> &vertices);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
