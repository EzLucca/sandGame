#pragma once

struct GLFWwindow;

class Renderer;
class Simulation;

class Player
{
    public:
        Player(GLFWwindow* window, float x, float y, float radius);

        void update(GLFWwindow *window, float deltaTime, const Simulation& simulation);
        void draw(Renderer& renderer);
        bool canMoveTo(float newX, float newY, const Simulation& simulation) const;
        float getX() const { return x; }
        float getY() const { return y; }

    private:
        GLFWwindow* window;
        float x;
        float y;

        float radius;
        float speed;
};

