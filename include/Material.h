#pragma once

enum class MaterialType
{
    Sand,
    Stone,
    Water,
    Fire,
    Smoke,
    WhiteBomb,
    RedBomb
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

inline const char* materialTypeName(MaterialType type)
{
    switch (type)
    {
        case MaterialType::Sand:  return "Sand";
        case MaterialType::Water: return "Water";
        case MaterialType::Smoke: return "Smoke";
        case MaterialType::Fire:  return "Fire";
        default:                  return "Unknown";
    }
}

