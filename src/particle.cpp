#include "Particle.h"
#include <cstdlib>

Particle::Particle()
{
    x = 0;
    y = 0;
    velocity = 0.0f;
}

Particle::Particle( int x, int y,
        const Material& material
        )
    : x(x),
    y(y),
    velocity(0.0f),
    material(material)
{ }

void Particle::applyGravity(float gravity, float deltaTime)
{
    velocity += gravity * deltaTime;
    float drag = 1.0f / (1.0f + material.viscosity * deltaTime);
    velocity *= drag;
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
    x-= (1.0f - material.viscosity);
}

void Particle::moveRight()
{
    x+= (1.0f + material.viscosity);
}

void Particle::moveDownLeft()
{
    x--;
    y++;
}

void Particle::moveDownRight()
{
    x++;
    y++;
}

void Particle::stop()
{
    velocity = 0.0f;
}

bool Particle::isAffectedByGravity() const
{
    return material.affectedByGravity;
}

bool Particle::isMovable() const
{
    return material.movable;
}

const Material& Particle::getMaterial() const
{
    return material;
}

float Particle::getSpread() const
{
    return material.spread;
}

void Particle::setPosition(int newX, int newY)
{
    x = newX;
    y = newY;
}
