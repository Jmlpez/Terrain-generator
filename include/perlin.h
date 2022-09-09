#ifndef __PERLIN_H__
#define __PERLIN_H__

/*
Perlin Noise Algorithm for Height Map
*/

#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>

using namespace std;

class Vector2D
{
public:
    Vector2D(){};
    Vector2D(const Vector2D &V)
    {
        this->first = V.first;
        this->second = V.second;
    }
    Vector2D(float a, float b)
    {
        first = a;
        second = b;
    }
    float dot(Vector2D V)
    {
        return this->first * V.first + this->second * V.second;
    }

private:
    float first, second;
};

void random(vector<int> &V);

void fillPermutation(vector<int> &table);

void setUp();

Vector2D getGradient(int value);

// Linear interpolation , also know as lerp
float lerp(float a, float b, float t);

float fade(float t);

float perlinNoise(float x, float y);

#endif