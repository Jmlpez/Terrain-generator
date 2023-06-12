#include "../include/Terrain.h"

struct Default
{
    float frequency = 3.0f;
    float lacunarity = 2.0f;
    float persistance = 0.5f;
    float scale = 1.0f;
    float mapHeight = 3.5f;
    float distance = 0.1f;
    int layers = 5; // octaves

} defaultValue;

struct TerrainColors
{
    glm::vec3 water = glm::vec3(10, 10, 245) / 256.0f;
    glm::vec3 sandy = glm::vec3(210, 180, 140) / 256.0f;
    glm::vec3 beach = glm::vec3(238, 214, 175) / 256.0f;
    glm::vec3 terrain = glm::vec3(34, 139, 34) / 256.0f;
    glm::vec3 jungle = glm::vec3(0, 100, 0) / 256.0f;
    // glm::vec3 mountain = glm::vec3(164, 189, 125) / 256.0f;
    glm::vec3 mountain = glm::vec3(139, 137, 137) / 256.0f;
    glm::vec3 snow = glm::vec3(255, 250, 250) / 256.0f;
} terrainColors;

Terrain::Terrain(int _width, int _height) : width(_width), height(_height)
{
    frequency = defaultValue.frequency;
    lacunarity = defaultValue.lacunarity;
    persistance = defaultValue.persistance;
    scale = defaultValue.scale;
    distance = defaultValue.distance;
    mapHeight = defaultValue.mapHeight;
    layers = defaultValue.layers;
    dimension = max(width, height);

    // Save Last Values
    lastFreq = frequency;
    lastLacuranity = lacunarity;
    lastPersistance = persistance;
    lastScale = scale;
    lastDistance = distance;
    lastMapHeight = mapHeight;

    lastWidth = width;
    lastHeight = height;
    lastDimension = dimension;

    srand(time(0));
    resetSeed();
    resetOptions();
    terrainMesh.setUpMesh();
}

void Terrain::drawTerrain(Shader &shader)
{
    terrainMesh.Draw(shader);
}

void Terrain::checkUpdate()
{
    if (lastFreq != frequency)
    {
        setFrequency(frequency);
        terrainMesh.setUpMesh();
        lastFreq = frequency;
    }
    if (lastLacuranity != lacunarity)
    {
        setLacunarity(lacunarity);
        terrainMesh.setUpMesh();
        lastLacuranity = lacunarity;
    }
    if (lastPersistance != persistance)
    {
        setPersistance(persistance);
        terrainMesh.setUpMesh();
        lastPersistance = persistance;
    }
    if (lastLayers != layers)
    {
        setLayers(layers);
        terrainMesh.setUpMesh();
        lastLayers = layers;
    }
    if (lastDimension != dimension)
    {
        setDimension(dimension, dimension);
        terrainMesh.setUpMesh();
        lastDimension = dimension;
    }
    if (lastDistance != distance)
    {
        setDistance(distance);
        terrainMesh.setUpMesh();
        lastDistance = distance;
    }
    if (lastMapHeight != mapHeight)
    {
        setMapHeight(mapHeight);
        terrainMesh.setUpMesh();
        lastMapHeight = mapHeight;
    }
}

void Terrain::setWidth(int _width)
{
    width = _width;
    resetOptions();
}
void Terrain::setHeight(int _height)
{
    height = _height;
    generateHeightMap();
    generateNormals();
}

void Terrain::setDimension(int _width, int _height)
{
    width = _width;
    height = _height;
    resetOptions();
}

void Terrain::setFrequency(float _frequency)
{
    frequency = _frequency;
    generateTerrain(terrainPos);
    generateHeightMap();
    generateNormals();
}
void Terrain::setLacunarity(float _lacunarity)
{
    lacunarity = _lacunarity;
    generateTerrain(terrainPos);
    generateHeightMap();
    generateNormals();
}

void Terrain::setPersistance(float _persistance)
{
    persistance = _persistance;
    generateTerrain(terrainPos);
    generateHeightMap();
    generateNormals();
}

void Terrain::setLayers(int _layers)
{
    layers = _layers;
    generateTerrain(terrainPos);
    generateHeightMap();
    generateNormals();
}

void Terrain::setMapHeight(float _mapHeight)
{
    mapHeight = _mapHeight;
    generateHeightMap();
    generateNormals();
}
void Terrain::resetTerrain()
{
    vector<vector<GLfloat>> positions(height + 1, vector<GLfloat>(width + 1));
    terrainPos = positions;
    generateTerrain(terrainPos);
}

void Terrain::resetSeed()
{
    setUp();
    resetOptions();
    terrainMesh.setUpMesh();
}

void Terrain::resetOptions()
{
    // Vector to track the common vertices at one point Ex: (1,2)->{5,6,9,10}
    vector<vector<vector<GLuint>>> vert(height + 1, vector<vector<GLuint>>(width + 1));
    commonVert = vert;
    resetTerrain();
    generateVertices();
    generateIndices();
    generateHeightMap();
    generateNormals();
}

void Terrain::setDistance(float _dist)
{
    for (int posz = 0; posz <= height; posz++)
    {
        for (int posx = 0; posx <= width; posx++)
        {
            for (auto &v : commonVert[posz][posx])
            {
                terrainMesh.vertices[v].position.x = (distance * posx);
                terrainMesh.vertices[v].position.z = (distance * posz);
            }
        }
    }
    generateNormals();
}

void Terrain::generateVertices()
{
    int tamN = height + 1, tamM = width + 1;

    int numVert = (2 * (tamN - 1)) * (2 * (tamM - 1));

    vector<Vertex> vertices(numVert);

    // Generate Vertex
    for (int posz = 0, idx = 0; posz <= tamN - 1; posz++)
    {
        int lastN = 2 * (tamN - 1);
        int lastM = 2 * (tamM - 1);
        for (int posx = 0; posx <= tamM - 1; posx++)
        {
            vertices[idx].normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices[idx].color = glm::vec3(0.0f, 0.0f, 1.0f);
            vertices[idx].position = glm::vec3(distance * posx, 1.0f, distance * posz);

            // The corners
            if ((posz == 0 && posx == 0) || (posz == 0 && posx == tamM - 1) || (posz == tamN - 1 && posx == 0) || (posx == tamM - 1 && posz == tamN - 1))
            {
                commonVert[posz][posx].push_back(idx);
                idx++;
            }
            // (0, X) ^ (tamN, X)
            else if ((posz == 0 && posx < tamM - 1) || (posz == tamN - 1 && posx < tamM - 1))
            {
                vertices[idx + 1] = vertices[idx];
                commonVert[posz][posx].push_back(idx);
                commonVert[posz][posx].push_back(idx + 1);
                idx += 2;
            }
            // (X, 0) ^ (X, tamM)
            else if ((posx == 0 && posz < tamN - 1) || (posx == tamN - 1 && posz < tamN - 1))
            {
                vertices[idx + lastN] = vertices[idx];
                commonVert[posz][posx].push_back(idx);
                commonVert[posz][posx].push_back(idx + lastN);

                // Si estas al final de la columna salta en lastN indices sino ve al siguiente
                idx += (posx == tamN - 1) ? lastN + 1 : 1;
            }
            // The middle points (X,X)
            else if (posx > 0 && posx < tamM - 1 && posz > 0 && posz < tamN - 1)
            {
                vertices[idx + 1] = vertices[idx + lastN] = vertices[idx + lastN + 1] = vertices[idx];
                commonVert[posz][posx].push_back(idx);
                commonVert[posz][posx].push_back(idx + 1);
                commonVert[posz][posx].push_back(idx + lastN);
                commonVert[posz][posx].push_back(idx + lastN + 1);
                idx += 2;
            }
        }
    }
    terrainMesh.setVertices(vertices);
}
void Terrain::generateIndices()
{
    int tamN = height + 1, tamM = width + 1;
    int numInd = height * width * 6;
    vector<GLuint> ind(numInd);
    for (int row = 0, idx = 0; row < tamN - 1; row++)
    {
        int lastN = 2 * (tamN - 1);
        for (int col = 0, val = 2 * lastN * (row); col < tamM - 1; col++, val += 2)
        {
            // First triangle indices
            ind[idx++] = val;
            ind[idx++] = val + 1;
            ind[idx++] = val + lastN;

            // Second Triangle indices
            ind[idx++] = val + 1;
            ind[idx++] = val + lastN;
            ind[idx++] = val + lastN + 1;
        }
    }
    terrainMesh.setIndices(ind);
}

void Terrain::generateNormals()
{
    int tamN = height + 1, tamM = width + 1;
    // Assing the correct Normal Vector to each Face (Flat Shading)
    for (int row = 0, idx = 0; row < tamN - 1; row++)
    {
        int lastN = 2 * (tamN - 1);
        for (int col = 0, val = 2 * lastN * (row); col < tamM - 1; col++, val += 2)
        {

            glm::vec3 normal = getNormalVector(terrainMesh.vertices[val].position, terrainMesh.vertices[val + 1].position, terrainMesh.vertices[val + lastN].position);
            terrainMesh.vertices[val].normal = normal;
            terrainMesh.vertices[val + 1].normal = normal;
            terrainMesh.vertices[val + lastN].normal = normal;
            terrainMesh.vertices[val + lastN + 1].normal = normal;
        }
    }
    // for smooth shading
    for (int posz = 0; posz <= height; posz++)
    {
        for (int posx = 0; posx <= width; posx++)
        {
            glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
            for (auto &v : commonVert[posz][posx])
            {
                normal += terrainMesh.vertices[v].normal;
            }
            normal = glm::normalize(normal);
            for (auto &v : commonVert[posz][posx])
            {
                terrainMesh.vertices[v].normal = normal;
            }
        }
    }
}

void Terrain::generateHeightMap()
{

    // The noise value is used for the color of the vertice

    // Reset the -Y- component for the height map
    // for (auto &v : terrainMesh.vertices)
    //     v.position.y = 1.0f;

    // Assing the corresponding height to the Y component of the vertices
    for (unsigned int posz = 0, idx = 0; posz < terrainPos.size(); posz++)
    {
        for (unsigned int posx = 0; posx < terrainPos[0].size(); posx++)
        {
            float noise = terrainPos[posz][posx];
            for (auto v : commonVert[posz][posx])
            {
                terrainMesh.vertices[v].position.y = 1.0f + (noise * mapHeight);

                terrainMesh.vertices[v].color = getColor(noise);
            }
        }
    }
}

glm::vec3 Terrain::getColor(float noise)
{
    glm::vec3 color;
    if (noise < 0.2f) // Water
        color = terrainColors.water;
    else if (noise < 0.23f) // Sand
        color = terrainColors.sandy;
    else if (noise < 0.26f) // Beach
        color = terrainColors.beach;
    else if (noise < 0.5f)
        color = terrainColors.terrain;
    else if (noise < 0.6f)
        color = terrainColors.jungle;
    else if (noise < 0.7f)
        color = terrainColors.mountain;
    else // Snow
        color = terrainColors.snow;
    return color;
}

glm::vec3 Terrain::getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3)
{
    glm::vec3 firstV = vert2 - vert1;
    glm::vec3 secondV = vert3 - vert1;

    return glm::normalize(glm::cross(firstV, secondV));
}

void Terrain::generateTerrain(vector<vector<GLfloat>> &positions)
{
    float maxNoiseValue = 0, totalAmp = 0;
    vector<GLfloat> Noises;
    for (int i = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < positions[0].size(); j++)
        {
            float totalNoise = 0, freq = frequency, amp = 1.0f;
            totalAmp = 0;
            for (int k = 0; k < layers; k++)
            {
                float waveLenght = dimension / freq;
                float noise = amp * (perlinNoise(i / waveLenght, j / waveLenght));
                totalNoise += noise;
                freq *= lacunarity;
                amp *= persistance;
                totalAmp += amp;
            }
            totalNoise += 1.0f;
            totalNoise *= 0.5f;
            maxNoiseValue = max(maxNoiseValue, totalNoise);
            positions[i][j] = totalNoise;
            // Noises.push_back(totalNoise);
        }
    }
    // Normalizing to get values between (0.0, 1.0)
    // for (float &n : Noises)
    //     n /= totalAmp;

    for (int i = 0, count = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < positions[0].size(); j++)
        {
            // positions[i][j] /= totalAmp;
            // positions[i][j] = pow(positions[i][j], 1.2f);
        }
    }
}
