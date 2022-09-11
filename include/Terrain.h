#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include "./Mesh.h"
#include "./perlin.h"

class Terrain
{
public:
    Terrain(GLuint _width = 20, GLuint _height = 20);

    void drawTerrain(Shader &shader);
    void checkUpdate();

    void
    generateVertices();
    void generateHeightMap();
    void generateIndices();
    void generateNormals();

    void resetSeed();
    void resetOptions();
    void resetTerrain();
    // Setters
    void setWidth(GLuint _width);
    void setHeight(GLuint _height);
    void setFrequency(float _frequency);
    void setAmplitude(float _amplitude);
    void setMapHeight(float _mapHeight);
    // Getters
    GLuint getWidth() { return width; }
    GLuint getheight() { return height; }
    float getFrequency() { return frequency; }
    float getAmplitude() { return amplitude; }
    float getMapHeight() { return mapHeight; }

    vector<vector<GLfloat>> terrainPos;

private:
    void generateTerrain(vector<vector<GLfloat>> &positions);
    glm::vec3 getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3);

private:
    vector<vector<vector<GLuint>>> commonVert;

    // Mesh
    Mesh terrainMesh;

public:
    GLuint width;
    GLuint height;
    GLuint layers;

    float frequency, amplitude;
    float scale;
    float mapHeight;

    float lastFreq, lastAmp, lastScale, lastMapHeight;
    GLuint lastWidth, lastHeight, lastLayers;
};

#endif