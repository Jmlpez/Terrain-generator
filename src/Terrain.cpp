#include "../include/Terrain.h"

struct Default
{
    float frequency = 0.04f;
    float amplitude = 1.0f;
    float scale = 1.0f;
    float mapHeight = 7.5f;

    GLuint layers = 5; // octaves

} defaultValue;

Terrain::Terrain(GLuint _width, GLuint _height) : width(_width), height(_height)
{
    frequency = defaultValue.frequency;
    amplitude = defaultValue.amplitude;
    scale = defaultValue.scale;
    mapHeight = defaultValue.mapHeight;
    layers = defaultValue.layers;

    // Save Last Values
    lastFreq = frequency;
    lastAmp = amplitude;
    lastScale = scale;
    lastMapHeight = mapHeight;

    lastWidth = width;
    lastHeight = height;

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
        // cout << lastFreq << "\n;
    }
}

void Terrain::setWidth(GLuint _width)
{
    width = _width;
    resetOptions();
}
void Terrain::setHeight(GLuint _height)
{
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
void Terrain::setAmplitude(float _amplitude)
{
    amplitude = _amplitude;
    resetOptions();
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
    // cout << terrainPos[3][3] << '\n';
}

void Terrain::resetSeed()
{
    srand(time(0));
    setUp();
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

void Terrain::generateVertices()
{
    int tamN = height + 1, tamM = width + 1;

    int numVert = (2 * (tamN - 1)) * (2 * (tamM - 1));

    vector<Vertex> vertices(numVert);

    // Generate Vertex
    for (int posz = 0, idx = 0; posz <= tamN - 1; posz++)
    {
        float distance = 0.1f;
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
}

void Terrain::generateHeightMap()
{

    // The noise value is used for the color of the vertice

    // Reset the -Y- component for the height map
    for (auto &v : terrainMesh.vertices)
        v.position.y = 1.0f;

    // Assing the corresponding height to the Y component of the vertices
    for (unsigned int posz = 0, idx = 0; posz < terrainPos.size(); posz++)
    {
        for (unsigned int posx = 0; posx < terrainPos[0].size(); posx++)
        {
            float noise = terrainPos[posz][posx];
            for (auto v : commonVert[posz][posx])
            {
                terrainMesh.vertices[v].position.y += noise * mapHeight * 0.4f;

                glm::vec3 color;
                float rgb = noise;
                if (noise < 0.4) // Water
                    color = glm::vec3(0.0f, 0.0f, 2 * rgb);
                else if (noise < 0.7) // Terrain
                    color = glm::vec3(0.0f, rgb, rgb * 0.5f);
                else // Mountain
                    color = glm::vec3(rgb, rgb, rgb);
                terrainMesh.vertices[v].color = color;
            }
        }
    }
}

glm::vec3 Terrain::getNormalVector(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3)
{
    glm::vec3 firstV = vert2 - vert1;
    glm::vec3 secondV = vert3 - vert1;

    return glm::normalize(glm::cross(firstV, secondV));
}

void Terrain::generateTerrain(vector<vector<GLfloat>> &positions)
{
    float maxNoiseValue = 0;
    vector<GLfloat> Noises;
    for (int i = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < positions[0].size(); j++)
        {
            float total = 0, freq = frequency, amp = amplitude;
            for (int k = 0; k < layers; k++)
            {
                float noise = amp * 0.5f * (perlinNoise(i * freq, j * freq) + 1.0f);
                total += noise;
                freq *= 2.0f;
                amp *= 0.5;
            }

            maxNoiseValue = max(maxNoiseValue, total);
            Noises.push_back(total);
        }
    }
    // Normalizing to get values between (0.0, 1.0)
    for (float &n : Noises)
        n /= maxNoiseValue;

    for (int i = 0, count = 0; i < positions.size(); i++)
    {
        for (int j = 0; j < positions[0].size(); j++)
        {
            positions[i][j] = Noises[count++];
        }
    }
}
