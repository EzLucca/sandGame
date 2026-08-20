#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
    protected:
        int x;
        int y;
        float velocity;

    public:
        Particle();
        Particle(int startX, int startY);

        void applyGravity(float gravity, float deltaTime);
        int getX();
        int getY();
        float getVelocity();

        void moveDown();
        void moveLeft();
        void moveRight();

        void stop();
};

#endif
