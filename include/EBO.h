#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include "./glm/glm.hpp"
#include <glad/glad.h>
#include <vector>

using namespace std;

class EBO
{
public:
    GLuint ID;
    EBO(vector<GLuint> &indices);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
