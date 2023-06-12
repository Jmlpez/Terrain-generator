#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include "./Mesh.h"
#include "./perlin.h"

class Terrain
{
public:
    Terrain(int _width = 20, int _height = 20);

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
    void setWidth(int _width);
    void setHeight(int _height);
    void setDimension(int _width, int _height);
    void setDistance(float _dist);
    void setFrequency(float _frequency);
    void setLayers(int _layers);
    void setLacunarity(float _lacunarity);
    void setPersistance(float _persistance);
    void setMapHeight(float _mapHeight);
    // Getters
    GLuint getWidth() { return width; }
    GLuint getheight() { return height; }
    float getFrequency() { return frequency; }
    float getLacunarity() { return lacunarity; }
    float getMapHeight() { return mapHeight; }

    vector<vector<GLfloat>> terrainPos;

private:
    void generateTerrain(vector<vector<GLfloat>> &positions);
    glm::vec3 getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3);
    glm::vec3 getColor(float noise);

private:
    vector<vector<vector<GLuint>>> commonVert;

    // Mesh
    Mesh terrainMesh;

public:
    int width;
    int height;
    int layers;
    int dimension;

    float frequency, persistance, lacunarity;
    float scale;
    float distance;
    float mapHeight;

    float lastFreq, lastLacuranity, lastPersistance, lastScale, lastDistance, lastMapHeight;
    int lastWidth, lastHeight, lastDimension, lastLayers;

    bool seed;
};

#endif