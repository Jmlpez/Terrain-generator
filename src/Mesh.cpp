#include "../include/Mesh.h"

Mesh::Mesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setUpMesh();
}
void Mesh::setUpMesh()
{
	this->VAO1.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	this->VAO1.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
	// VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float))); // Color is not used
	this->VAO1.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));
	this->VAO1.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, color)));

	// Unbind all to prevent accidentally modifying them
	this->VAO1.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw(Shader &shader)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	this->VAO1.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	// Exporting the textures uniforms correctly
	/*
	material.diffuse0
	material.diffuse1
	...
	material.specular0
	material.specular1
	*/
	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		string num;
		string type = this->textures[i].type;
		if (type == "diffuse")
		{
			num = to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = to_string(numSpecular++);
		}
		// Exports the textures to the shader
		textures[i].texUnit(shader, ("material." + (type + num)).c_str(), i);
		textures[i].Bind();
	}

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // To draw the mesh in wireframe mode
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // To draw the mesh in normal mode (default)

	// Draw the actual mesh, using EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}