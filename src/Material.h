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

    float r, g, b, a;
    bool isFire;
};
