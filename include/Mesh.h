#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>

using namespace std;

#include "VAO.h"
#include "EBO.h"
#include "camera.h"
#include "Texture.h"

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// Store VAO in public so it can be used in the Draw function
	VAO VAO1;

	// Initializes the mesh
	Mesh(){};
	Mesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures);
	void setVertices(vector<Vertex> _vertices) { vertices = _vertices; }
	void setIndices(vector<GLuint> _indices) { indices = _indices; }
	void setTextures(vector<Texture> _textures) { textures = _textures; }
	void setUpMesh();
	// Draws the mesh
	void Draw(Shader &shader);
};
#endif