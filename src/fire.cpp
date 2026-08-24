#include "Fire.h"
#include "Materials.h"
#include <cstdlib>

Fire::Fire()
    : Particle()
{
    age = 0.0f;
    lifetime = 1.0f;
    maxLifetime = 1.0f;
    horizontalVelocity = 0.0f;
}

Fire::Fire(int startX, int startY)
    : Particle(startX, startY, fireMaterial)
{
    age = 0.0f;
    lifetime = 1.2f;
    maxLifetime = 0.5f + static_cast<float>(rand() % 100) / 100.0f;
    horizontalVelocity = 0.0f;
}

void Fire::update(float gravity, float deltaTime)
{
    // -----------------------------------------
    // Lifetime
    // -----------------------------------------

    lifetime += deltaTime;

    // -----------------------------------------
    // Vertical movement
    // -----------------------------------------

    velocity += gravity * deltaTime;

    // y += velocity * deltaTime;

    // -----------------------------------------
    // Random horizontal drift
    // -----------------------------------------

    float randomDrift =
        static_cast<float>(rand() % 201) - 100.0f;

    horizontalVelocity +=
        randomDrift * 0.05f * deltaTime;

    // -----------------------------------------
    // Horizontal damping
    // -----------------------------------------

    horizontalVelocity *= 0.98f;

    // -----------------------------------------
    // Limit horizontal velocity
    // -----------------------------------------

    if (horizontalVelocity > 50.0f)
        horizontalVelocity = 50.0f;

    if (horizontalVelocity < -50.0f)
        horizontalVelocity = -50.0f;

    // -----------------------------------------
    // Horizontal movement
    // -----------------------------------------

    // x += horizontalVelocity * deltaTime;
}

bool Fire::isDead()
{
    return lifetime >= maxLifetime;
}

void Fire::setVelocity(float value)
{
    velocity = value;
}

void Fire::setHorizontalVelocity(float value)
{
    horizontalVelocity = value;
}

float Fire::getHorizontalVelocity() const
{
    return horizontalVelocity;
}

float Fire::getVelocity() const
{
    return velocity;
}
