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
        bool scheduled = false;
        int activeIndex = -1;

    public:
        Particle();
        Particle(int startX, int startY, const Material& material);
        const Material& getMaterial() const;

        bool hasLifetime() const;
        bool isActive() const;
        bool isAffectedByGravity() const;
        bool isDead() const;
        bool isMovable() const;
        bool isScheduled() const;

        float getHorizontalVelocity() const;
        float getLifetime() const;
        float getSpread() const;
        float getVelocity() const;

        int getActiveIndex() const;
        int getX() const;
        int getY() const;

        void applyGravity(int gravity, float deltaTime);
        void move(int dx, int dy);
        void moveHorizontal(int direction);
        void moveUp();
        void moveUpLeft();
        void moveUpRight();
        void moveVertical(int direction);
        void setActive(bool value);
        void setActiveIndex(int index);
        void setHorizontalVelocity(float value);
        void setLifetime(float value);
        void setPosition(int newX, int newY);
        void setScheduled(bool value);
        void setVelocity(float value);
        void stop();
        void updateLifetime(float deltaTime);
};

