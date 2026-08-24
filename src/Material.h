#pragma once

struct Material
{
    float viscosity;
    float density;
    float spread;
    bool affectedByGravity;
    bool movable;

    float r;
    float g;
    float b;
    float a;
    bool isFire;
};

