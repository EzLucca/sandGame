#include "Fire.h"
#include "Materials.h"

Fire::Fire()
    : Particle()
{
    age = 0.0f;
    lifetime = 1.0f;
}

Fire::Fire(int startX, int startY)
    : Particle(startX, startY, fireMaterial)
{
    age = 0.0f;
    lifetime = 1.0f;
    horizontalVelocity = 0.0f;
}

void Fire::update(float gravity, float deltaTime)
{
    age += deltaTime;

    velocity += gravity * deltaTime;

    y += static_cast<int>(velocity * deltaTime);
    x += static_cast<int>(horizontalVelocity * deltaTime);
}

bool Fire::isDead()
{
    return age >= lifetime;
}

void Fire::setVelocity(float value)
{
    velocity = value;
}

void Fire::setHorizontalVelocity(float value)
{
    horizontalVelocity = value;
}
