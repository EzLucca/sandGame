#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Player.h"
#include "Renderer.h"
#include "Simulation.h"

#include <cmath>

Player::Player(
        GLFWwindow* window,
        float x,
        float y,
        float radius)
    : window(window),
    x(x),
    y(y),
    radius(radius),
    speed(200.0f)
{
}

void Player::update(GLFWwindow *window, float deltaTime, const Simulation& simulation)
{
    float dx = 0.0f;
    float dy = 0.0f;

    // Horizontal movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        dx -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        dx += 1.0f;
    }

    // Vertical movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        dy -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        dy += 1.0f;
    }

    // Prevent diagonal movement from being faster
    float length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.0f)
    {
        dx /= length;
        dy /= length;
    }

    float movementX = dx * speed * deltaTime;
    float movementY = dy * speed * deltaTime;

    // X movement
    float newX = x + movementX;

    if (canMoveTo(newX, y, simulation))
        x = newX;

    // Y movement
    float newY = y + movementY;

    if (canMoveTo(x, newY, simulation))
        y = newY;
}

void Player::draw(Renderer& renderer)
{
    renderer.drawCircle(
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(radius),
            1.0f,
            0.0f,
            1.0f
            );
}

bool Player::canMoveTo( float newX, float newY,
    const Simulation& simulation) const
{
    int minX = static_cast<int>(std::floor(newX - radius));
    int maxX = static_cast<int>(std::ceil(newX + radius));

    int minY = static_cast<int>(std::floor(newY - radius));
    int maxY = static_cast<int>(std::ceil(newY + radius));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Outside simulation
            if (x < 0 ||
                x >= Simulation::WIDTH ||
                y < 0 ||
                y >= Simulation::HEIGHT)
            {
                return false;
            }

            float dx = static_cast<float>(x) - newX;
            float dy = static_cast<float>(y) - newY;

            // Is this pixel inside the player's circle?
            if (dx * dx + dy * dy <= radius * radius)
            {
                if (simulation.isOccupied(x, y))
                    return false;
            }
        }
    }

    return true;
}

