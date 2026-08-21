#ifndef PARTICLE_H
#define PARTICLE_H

#include "Material.h"

class Particle
{
    protected:
        int x;
        int y;
        float velocity;
        Material material;

    public:
        Particle();
        Particle(int startX, int startY, const Material& material);
        const Material& getMaterial() const;

        void applyGravity(float gravity, float deltaTime);

        void moveDown();
        void moveLeft();
        void moveRight();
        void moveDownLeft();
        void moveDownRight();
        void stop();

        int getX();
        int getY();
        float getVelocity();
        bool isAffectedByGravity() const;
        bool isMovable() const;

};

#endif
