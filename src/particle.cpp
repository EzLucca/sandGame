#include "Particle.h"

Particle::Particle()
{
    x = 0;
    y = 0;
    velocity = 0.0f;
}

Particle::Particle(int startX, int startY)
{
    x = startX;
    y = startY;
    velocity = 0.0f;
}

void Particle::applyGravity(float gravity, float deltaTime)
{
    velocity += gravity * deltaTime;
}

int Particle::getX()
{
    return x;
}

int Particle::getY()
{
    return y;
}

float Particle::getVelocity()
{
    return velocity;
}
void Particle::moveDown()
{
    y++;
}

void Particle::moveLeft()
{
    x--;
    y++;
}

void Particle::moveRight()
{
    x++;
    y++;
}

void Particle::stop()
{
    velocity = 0.0f;
}
