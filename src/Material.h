#pragma once

struct Material
{
    float viscosity;
    float density;
    float spread;
    bool affectedByGravity;
    int dir;
    float gravityValue;
    bool movable;
    float lifetime;

    float r;
    float g;
    float b;
    float a;
    bool isFire;
};

// enum class MaterialType
// {
//     Air,
//     Sand,
//     Water,
//     Rock,
//     Smoke,
//     Fire
// };
//
// enum class MaterialCategory
// {
//     Empty,
//     Gas,
//     Liquid,
//     Powder,
//     Solid
// };
//
// struct Vector2i
// {
//     int x;
//     int y;
// };
//
// struct MaterialProperties
// {
//     MaterialCategory category;
//     Vector2i gravityDirection;
//
//     float density;
//     int spread;
//     float heat;
//     bool movable;
//
//     float r;
//     float g;
//     float b;
//     float a;
//     bool isFire;
// };
//
// const MaterialProperties& getProperties(MaterialType type);
