#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Particle.h"
#include "Fire.h"
#include "Materials.h"
#include "MaterialSelector.h"
#include "Mouse.h"
#include "Renderer.h"
#include "Movements.h"
#include "Simulation.h"
#include "Player.h"


std::string readFile(const char* path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Could not open: " << path << '\n';
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    Mouse* mouse =
        static_cast<Mouse*>(glfwGetWindowUserPointer(window));

    if (mouse)
        mouse->scroll(yOffset);
}
struct Bullet
{
    bool active = false;

    float x = 0.0f;
    float y = 0.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;
};

static Bullet bullet;

void shootBullet(
        Simulation& simulation,
        float startX,
        float startY,
        float directionX,
        float directionY)
{
    if (bullet.active)
        return;

    // Normalize direction
    float length = std::sqrt(
            directionX * directionX +
            directionY * directionY
            );

    if (length == 0.0f)
        return;

    directionX /= length;
    directionY /= length;

    constexpr float speed = 500.0f;

    bullet.active = true;

    bullet.x = startX;
    bullet.y = startY;

    bullet.velocityX = directionX * speed;
    bullet.velocityY = directionY * speed;
}

void updateBullet( Simulation& simulation, float deltaTime)
{
    if (!bullet.active)
        return;

    bullet.x += bullet.velocityX * deltaTime;
    bullet.y += bullet.velocityY * deltaTime;

    int x = static_cast<int>(bullet.x);
    int y = static_cast<int>(bullet.y);

    // Hit another particle
    if (x < 0 || x >= Simulation::WIDTH ||
            y < 0 || y >= Simulation::HEIGHT)
    {
        bullet.active = false;
        return;
    }

    // Check if bullet hit a particle
    int particleIndex = simulation.getParticleIndexAt(x, y);

    if (particleIndex != -1)
    {
        simulation.removeParticle(particleIndex);

        bullet.active = false;
        return;
    }
}

int main()
{
    Simulation simulation;

    // ----- Initialize GLFW -----

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ----- Create window -----

    GLFWwindow* window = glfwCreateWindow(
            Simulation::WIDTH * Simulation::SCALE,
            Simulation::HEIGHT  * Simulation::SCALE,
            "Pixel simulation",
            nullptr,
            nullptr
            );

    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    Player player(window, Simulation::WIDTH / 2.0f, Simulation::HEIGHT / 2.0f, 10.0f);

    // ----- setting mouse -----

    Mouse mouse( window, Simulation::WIDTH, Simulation::HEIGHT);
    glfwSetWindowUserPointer(window, &mouse);
    glfwSetScrollCallback(window, scroll_callback);

    MaterialSelector materialSelector(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // ----- Initialize GLAD -----

    if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    {
        Renderer renderer(Simulation::WIDTH, Simulation::HEIGHT);

        // ----- Particle arrays -----

        float lastTime = glfwGetTime();
        double fpsTimer = glfwGetTime();
        int frameCount = 0;

        // ----- Main loop -----

        while (!glfwWindowShouldClose(window))
        {
            // ----- Material selection -----

            const Material* newMaterial = materialSelector.update();

            if (newMaterial != nullptr)
                simulation.setSelectedMaterial(newMaterial);

            // ----- Mouse -----

            mouse.update();

            int gridX = mouse.getX();
            int gridY = mouse.getY();

            if (mouse.isLeftPressed())
                simulation.useBrush( gridX, gridY, mouse.getBrushRadius(), false);

            else if (mouse.isErasePressed())
                simulation.useBrush( gridX, gridY, mouse.getBrushRadius(), true);

            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            {
                Movements::triggerExplosion(simulation, gridX, gridY,
                        mouse.getBrushRadius());
            }

            if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            {
                Movements::launchParticle( simulation, player.getX(), player.getY());
            }
            if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            {
                float dx = gridX - player.getX();
                float dy = gridY - player.getY();

                shootBullet( simulation, player.getX(), player.getY(), dx, dy);
            }
            if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
                simulation.setGravity(-simulation.getGravity());

            // ----- DEBUG MOVEMENTS -----
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
                simulation.diagonalMoves = !simulation.diagonalMoves;
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
                simulation.horizontalMoves = !simulation.horizontalMoves;
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
                simulation.verticalMoves = !simulation.verticalMoves;
            // ----- DEBUG MOVEMENTS -----

            // Clear all particles when 'C' key is pressed
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
                simulation.clearAll();

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                break;

            // ----- Time -----

            float currentTime = glfwGetTime();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // ----- Simulation -----

            simulation.update(deltaTime);
            updateBullet(simulation, deltaTime);
            Movements::updateLaunch( simulation, deltaTime);
            Movements::updateExplosion(simulation);
            player.update(window, deltaTime, simulation);
            renderer.render(simulation.getPixelData());
            player.draw(renderer);

            // ----- Bullet draw -----
            if (bullet.active)
            {
                renderer.drawCircle(
                        static_cast<int>(bullet.x),
                        static_cast<int>(bullet.y),
                        2,
                        1.0f,
                        1.0f,
                        1.0f
                        );
            }

            // ----- Brush preview -----
            renderer.drawCircunference( mouse.getX(), mouse.getY(), mouse.getBrushRadius(),
                    1.0f, 1.0f, 1.0f);

            // ----- Lauch -----
            if (Movements::isLaunchActive())
            {
                renderer.drawCircle(
                        Movements::getLaunchX(),
                        Movements::getLaunchY(),
                        4,
                        1.0f,
                        1.0f,
                        1.0f
                        );
            }

            // ----- Draw explosion -----
            if (Movements::isExplosionActive())
            {
                if (Movements::getExplosionStage() == ExplosionStage::White)
                {
                    renderer.drawCircle(
                            Movements::getExplosionX(),
                            Movements::getExplosionY(),
                            Movements::getExplosionRadius(),
                            1.0f, 1.0f, 1.0f
                            );
                }
                else if (Movements::getExplosionStage() == ExplosionStage::Red)
                {
                    renderer.drawCircle(
                            Movements::getExplosionX(),
                            Movements::getExplosionY(),
                            Movements::getExplosionRadius(),
                            1.0f, 0.0f, 0.0f
                            );
                }
            }

            // ----- Upload pixel buffer to texture -----
            glfwSwapBuffers(window);

            // ----- update title -----

            frameCount++;

            double currentFPS = glfwGetTime();

            if (currentFPS - fpsTimer >= 0.5)
            {
                double elapsed = currentFPS - fpsTimer;
                double fps = frameCount / elapsed;

                std::string title = "Pixel simulation | Particles: " +
                    std::to_string(simulation.getParticleCount()) + " | Moving: " +
                    std::to_string(simulation.getActiveParticleCount()) + " | FPS: " +
                    std::to_string(fps);

                glfwSetWindowTitle( window, title.c_str());

                frameCount = 0;
                fpsTimer = currentFPS;
            }
            glfwPollEvents();
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}

