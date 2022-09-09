#include "../include/Model.h"

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | !aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
        // ASSERT(false);
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node’s meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // process vertex positions, normals and texture coordinates
        Vertex vertex;
        glm::vec3 vect;
        // Process vertex positions
        vect.x = mesh->mVertices[i].x;
        vect.y = mesh->mVertices[i].y;
        vect.z = mesh->mVertices[i].z;
        vertex.position = vect;

        // Process Normal vectors
        vect.x = mesh->mNormals[i].x;
        vect.y = mesh->mNormals[i].y;
        vect.z = mesh->mNormals[i].z;
        vertex.normal = vect;

        // Process the texture coordinates
        // A model can have up to 8 texture coordinates per vertice, we just use 1
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 tex;
            tex.x = mesh->mTextureCoords[0][i].x;
            tex.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = tex;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", 0);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", (unsigned int)textures.size());
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, GLuint slotNumber)
{
    vector<Texture> newTextures;
    cout << typeName << " " << mat->GetTextureCount(type) << '\n';
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        string texPath = directory + '/' + str.C_Str();
        for (unsigned int j = 0; j < loadedTextures.size(); j++)
        {
            // if the texture has been loaded, just save it
            if (strcmp(loadedTextures[j].path.data(), texPath.c_str()) == 0)
            {
                newTextures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }
        // if the texture hasn't been loaded already, load it
        if (!skip)
        {
            // To set the corresponding unit for binding it later
            GLuint slot = slotNumber + i;
            Texture texture(texPath.c_str(), typeName.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);

            newTextures.push_back(texture);
            // save the new texture for prevent re-loading it
            loadedTextures.push_back(texture);
        }
    }
    return newTextures;
}
