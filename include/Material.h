#pragma once

enum class MaterialType
{
    Sand,
    Stone,
    Water,
    Fire,
    Smoke
};

struct Material
{
    MaterialType type;
    float viscosity;
    float density;
    float spread;
    bool affectedByGravity;
    int dir;
    float gravityValue;
    bool movable;
    float lifetime;

    float r, g, b, a;
    bool isFire;
};
