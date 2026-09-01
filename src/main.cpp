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

int main()
{
    Simulation simulation;

    // --------------------------------------------------
    // 1. Initialize GLFW
    // --------------------------------------------------

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // --------------------------------------------------
    // 2. Create window
    // --------------------------------------------------

    GLFWwindow* window = glfwCreateWindow(
            Simulation::WIDTH,
            Simulation::HEIGHT,
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

    Mouse mouse( window, Simulation::WIDTH, Simulation::HEIGHT);
    MaterialSelector materialSelector(window);

    glfwMakeContextCurrent(window);


    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // --------------------------------------------------
    // 3. Initialize GLAD
    // --------------------------------------------------

    if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    {
        Renderer renderer(Simulation::WIDTH, Simulation::HEIGHT);

        // --------------------------------------------------
        // Particle arrays
        // --------------------------------------------------

        float lastTime = glfwGetTime();
        double fpsTimer = glfwGetTime();
        int frameCount = 0;

        // --------------------------------------------------
        // 10. Main loop
        // --------------------------------------------------

        while (!glfwWindowShouldClose(window))
        {
            // ---------------------------------------------
            // Material selection
            // ---------------------------------------------

            const Material* newMaterial = materialSelector.update();

            if (newMaterial != nullptr)
            {
                // selectedMaterial = newMaterial;
                simulation.setSelectedMaterial(newMaterial);
            }

            // ---------------------------------------------
            // Mouse
            // ---------------------------------------------
            mouse.update();

            int gridX = mouse.getX();
            int gridY = mouse.getY();

            constexpr int BRUSH_RADIUS = 8;


            if (mouse.isLeftPressed())
            {
                simulation.useBrush( gridX, gridY, BRUSH_RADIUS, false);
            }

            else if (mouse.isErasePressed())
            {
                simulation.useBrush( gridX, gridY, BRUSH_RADIUS, true);
            }

            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            {
                Movements::drawTemporaryCircle(simulation, 200, 150, 15, 0.5f);
            }
            if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            {
                simulation.setGravity(-simulation.getGravity());
            }

            // Clear all particles when 'C' key is pressed
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            {
                simulation.clearAll();
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                break;
            }

            // ---------------------------------------------
            // Time
            // ---------------------------------------------

            float currentTime = glfwGetTime();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            // ---------------------------------------------
            // Simulation
            // ---------------------------------------------

            simulation.update(deltaTime);

            // ---------------------------------------------
            // Upload pixel buffer to texture
            // ---------------------------------------------
            renderer.render(simulation.getPixelData());
            glfwSwapBuffers(window);

            // ---------------------------------------------
            // update title 
            // ---------------------------------------------

            frameCount++;

            double currentFPS = glfwGetTime();

            if (currentFPS - fpsTimer >= 0.5)
            {
                double fps = frameCount / (currentFPS - fpsTimer);

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
