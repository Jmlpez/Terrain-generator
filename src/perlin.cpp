#include "../include/perlin.h"

vector<int> PT;

void random(vector<int> &V)
{
    srand(time(0));
    vector<int> tab = V;
    for (size_t i = 0; i < V.size(); i++)
    {
        int randPos = rand() % tab.size();
        V[i] = tab[randPos];
        tab.erase(tab.begin() + randPos);
    }
}
void fillPermutation(vector<int> &table)
{
    for (int i = 0; i < 256; i++)
        table.push_back(i);

    random(table);

    for (int i = 0; i < 256; i++)
        table.push_back(table[i]);
}

void setUp()
{
    fillPermutation(PT);
}

Vector2D getGradient(int value)
{
    Vector2D grad;
    switch (value & 3)
    {
    case 0:
        grad = Vector2D(1.0f, 1.0f);
        break;
    case 1:
        grad = Vector2D(-1.0f, 1.0f);
        break;
    case 2:
        grad = Vector2D(-1.0f, -1.0f);
        break;
    default:
        grad = Vector2D(1.0f, -1.0f);
        break;
    }
    return grad;
}

// Linear interpolation , also know as lerp
float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float fade(float t)
{
    return ((6 * t - 15) * t + 10) * t * t * t;
}

float perlinNoise(float x, float y)
{
    // cout << x << " " << y << '\n';
    int X = floor(x);
    int Y = floor(y);

    X &= 255;
    Y &= 255;

    float xf = x - floor(x);
    float yf = y - floor(y);

    // pointing vectors
    Vector2D bLeft(xf, yf);
    Vector2D bRight(xf - 1.0f, yf);
    Vector2D tLeft(xf, yf - 1.0f);
    Vector2D tRight(xf - 1.0f, yf - 1.0f);

    // Random values for gradient vectors
    int bLeftGrad = PT[PT[X] + Y];
    int bRightGrad = PT[PT[X + 1] + Y];
    int tLeftGrad = PT[PT[X] + Y + 1];
    int tRightGrad = PT[PT[X + 1] + Y + 1];

    // the dot products of the gradient and pointing vector of each corner of the square
    float dotTRight = tRight.dot(getGradient(tRightGrad));
    float dotTLeft = tLeft.dot(getGradient(tLeftGrad));
    float dotBRight = bRight.dot(getGradient(bRightGrad));
    float dotBLeft = bLeft.dot(getGradient(bLeftGrad));

    // Fade the points using an ease curve
    float u = fade(xf);
    float v = fade(yf);

    // Linear Interpolation
    float AB = lerp(dotBLeft, dotBRight, u);
    float CD = lerp(dotTLeft, dotTRight, u);

    float value = lerp(AB, CD, v);

    return value;
}