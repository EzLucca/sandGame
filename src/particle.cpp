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
    material(material),
    lifetime(material.lifetime)

{ }

void Particle::applyGravity(float gravity, float deltaTime)
{
    velocity += gravity * deltaTime;
    float drag = 1.0f / (1.0f + material.viscosity * deltaTime);
    velocity *= drag;
}

int Particle::getX() const
{
    return x;
}

int Particle::getY() const
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
void Particle::moveUp()
{
    y--;
}

void Particle::moveUpLeft()
{
    y--;
    x--;
}

void Particle::moveUpRight()
{
    y--;
    x++;
}

void Particle::move(int dx, int dy)
{
    x += dx;
    y += dy;
}

void Particle::moveHorizontal(int direction)
{
    x += direction;
}

void Particle::moveVertical(int direction)
{
    y += direction;
}
void Particle::setVelocity(float value)
{
    velocity = value;
}

float Particle::getVelocity() const
{
    return velocity;
}

void Particle::setHorizontalVelocity(float value)
{
    horizontalVelocity = value;
}

float Particle::getHorizontalVelocity() const
{
    return horizontalVelocity;
}
void Particle::setLifetime(float value)
{
    lifetime = value;
}

float Particle::getLifetime() const
{
    return lifetime;
}

bool Particle::hasLifetime() const
{
    return lifetime >= 0.0f;
}

void Particle::updateLifetime(float deltaTime)
{
    if (lifetime < 0.0f)
        return;

    lifetime -= deltaTime;

    if (lifetime < 0.0f)
        lifetime = 0.0f;
}

bool Particle::isDead() const
{
    return lifetime >= 0.0f && lifetime <= 0.0f;
}

void Particle::setMoved(bool value)
{
    moved = value;
}
bool Particle::hasMoved() const
{
    return moved;
}
