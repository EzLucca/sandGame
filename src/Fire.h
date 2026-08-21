#ifndef FIRE_H
#define FIRE_H

#include "Particle.h"
#include "Material.h"

class Fire : public Particle
{
    private:
        float age;
        float lifetime;
        float horizontalVelocity;

    public:
        Fire();
        Fire(int startX, int startY);

        void update(float gravity, float deltaTime);
        void setVelocity(float value);
        void setHorizontalVelocity(float value);
        bool isDead();
};

#endif
