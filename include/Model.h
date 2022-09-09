#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <string>
#include <vector>
// Model Importer Library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #include "./Mesh.h"
#include "./Mesh.h"

using namespace std;

class Model
{
public:
	Model(string path)
	{
		loadModel(path);
	}
	void Draw(Shader &shader);
	// model data
	vector<Mesh> meshes;

private:
	// Loaded textures
	vector<Texture> loadedTextures;
	string directory;

	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, GLuint slotNumber);
};

#endif