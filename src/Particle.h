#pragma once

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
        void moveUp();
        void moveUpLeft();
        void moveUpRight();
        void stop();
        void setPosition(int newX, int newY);

        int getX();
        int getY();
        float getVelocity();
        bool isAffectedByGravity() const;
        bool isMovable() const;
        float getSpread() const;

        void move(int dx, int dy);
        void moveHorizontal(int direction);
        void moveVertical(int direction);
};

