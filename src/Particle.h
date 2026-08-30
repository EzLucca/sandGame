#pragma once

#include "Material.h"

class Particle
{
    protected:
        int x;
        int y;
        float velocity;
        float horizontalVelocity;
        float lifetime = -1.0f;
        Material material;
        bool active = false;
        int activeIndex = -1;

    public:
        Particle();
        Particle(int startX, int startY, const Material& material);
        const Material& getMaterial() const;

        void applyGravity(int gravity, float deltaTime);

        // void moveDown();
        // void moveLeft();
        // void moveRight();
        // void moveDownLeft();
        // void moveDownRight();
        void moveUp();
        void moveUpLeft();
        void moveUpRight();
        void stop();
        void setPosition(int newX, int newY);

        int getX() const;
        int getY() const;
        bool isAffectedByGravity() const;
        bool isMovable() const;
        float getSpread() const;

        void move(int dx, int dy);
        void moveHorizontal(int direction);
        void moveVertical(int direction);
        void setVelocity(float value);
        float getVelocity() const;
        void setHorizontalVelocity(float value);
        float getHorizontalVelocity() const;
        void setLifetime(float value);
        float getLifetime() const;
        bool hasLifetime() const;
        void updateLifetime(float deltaTime);
        bool isDead() const;

        void setActive(bool value);
        bool isActive() const;
        int getActiveIndex() const;
        void setActiveIndex(int index);
};

