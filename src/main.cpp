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
#include "Mouse.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

int occupied[HEIGHT][WIDTH];

const Material* selectedMaterial = &sandMaterial;

const int PARTICLE_COUNT = 1000000;

Particle particles[PARTICLE_COUNT];

int particleCount = 0;
std::vector<int> activeParticles;
std::vector<int> nextActiveParticles;

std::vector<unsigned char> pixelData( WIDTH * HEIGHT * 4, 0);

inline unsigned int randomState = 123456789;

inline unsigned int fastRandom()
{
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;

    return randomState;
}

inline void setPixel(int x, int y, const Material& material)
{
    if (x < 0 || x >= WIDTH ||
            y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = static_cast<unsigned char>(material.r * 255.0f);
    pixelData[index + 1] = static_cast<unsigned char>(material.g * 255.0f);
    pixelData[index + 2] = static_cast<unsigned char>(material.b * 255.0f);
    pixelData[index + 3] = static_cast<unsigned char>(material.a * 255.0f);
}

inline void clearPixel(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    int index = (y * WIDTH + x) * 4;

    pixelData[index + 0] = 0;
    pixelData[index + 1] = 0;
    pixelData[index + 2] = 0;
    pixelData[index + 3] = 255;
}

void clearOccupied()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            occupied[y][x] = -1;
        }
    }
}


bool canDisplace(int particleIndex, int otherIndex)
{
    // Empty cell
    if (otherIndex == -1)
    {
        return true;
    }

    float myDensity =
        particles[particleIndex].getMaterial().density;

    float otherDensity =
        particles[otherIndex].getMaterial().density;

    return myDensity > otherDensity;
}

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

void activateParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    Particle& p = particles[index];

    if (p.isActive())
        return;

    p.setActive(true);

    nextActiveParticles.push_back(index);
}

void deactivateParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    particles[index].setActive(false);
}

void wakeNeighbors(int x, int y)
{
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= WIDTH ||
                    ny < 0 || ny >= HEIGHT)
                continue;

            int index = occupied[ny][nx];

            if (index == -1)
                continue;

            if (!particles[index].isMovable())
                continue;

            activateParticle(index);
        }
    }
}

void moveParticle(int index, int newX, int newY)
{
    Particle& p = particles[index];

    int oldX = p.getX();
    int oldY = p.getY();

    occupied[oldY][oldX] = -1;
    clearPixel(oldX, oldY);

    wakeNeighbors(oldX, oldY);
    p.setPosition(newX, newY);

    occupied[newY][newX] = index;
    setPixel(newX, newY, p.getMaterial());
}

void placeParticle(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    if (particleCount >= PARTICLE_COUNT)
        return;

    if (occupied[y][x] != -1)
        return;

    // saving the particle on index
    int newIndex = particleCount++;
    particles[newIndex] = Particle( x, y, *selectedMaterial);

    // Fire

    if (selectedMaterial->isFire)
    {
        particles[newIndex].setVelocity(
                -(400.0f + fastRandom() % 150)
                );

        particles[newIndex].setHorizontalVelocity(
                static_cast<float>(
                    static_cast<int>(fastRandom() % 61) - 30
                    )
                );

        particles[newIndex].setLifetime(
                1.0f + (fastRandom() % 100) / 100.0f
                );
    }

    occupied[y][x] = newIndex;

    // Draw the particle directly into the pixel buffer.
    setPixel( x, y, *selectedMaterial);

    activateParticle(newIndex);
}

// Compile a shader
GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog( shader, 512, nullptr, infoLog);

        std::cerr << "Shader compilation failed:\n" << infoLog << '\n';
    }

    return shader;
}

void removeParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    Particle& p = particles[index];

    int x = p.getX();
    int y = p.getY();

    if (x >= 0 && x < WIDTH &&
            y >= 0 && y < HEIGHT)
    {
        if (occupied[y][x] == index)
        {
            occupied[y][x] = -1;
            clearPixel(x, y);

            // Something above may now fall.
            wakeNeighbors(x, y);
        }
    }

    p.setActive(false);
    p.setPosition(-1, -1);
}

void useBrush( int centerX, int centerY, int radius, bool erase)
{
    for (int dx = -radius; dx <= radius; dx++)
    {
        for (int dy = -radius; dy <= radius; dy++)
        {
            if (dx * dx + dy * dy > radius * radius)
            {
                continue;
            }

            int x = centerX + dx;
            int y = centerY + dy;

            if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
            {
                continue;
            }

            if (erase)
            {
                int index = occupied[y][x];

                if (index != -1)
                {
                    removeParticle(index);
                }
            }
            else
            {
                placeParticle(x, y);
            }
        }
    }
}

void updateParticle( Particle& p, int index, float deltaTime)
{
    // -----------------------------------------
    // Gravity
    // -----------------------------------------

    if (p.isAffectedByGravity())
    {
        p.applyGravity( p.getMaterial().gravityValue, deltaTime);
    }

    // -----------------------------------------
    // Calculate vertical movement
    // -----------------------------------------

    float movement = p.getVelocity() * deltaTime;

    int steps = static_cast<int>(std::abs(movement));

    steps = std::clamp(steps, 1, 4);

    int direction = p.getMaterial().dir;

    // -----------------------------------------
    // Vertical + diagonal movement
    // -----------------------------------------

    bool blocked = false;

    for (int step = 0; step < steps; step++)
    {
        int x = p.getX();
        int y = p.getY();

        int nextY = y + direction;

        // -------------------------------------
        // Outside screen
        // -------------------------------------

        if (nextY < 0 || nextY >= HEIGHT)
        {
            if (p.getMaterial().isFire)
            {
                removeParticle(index);
                return;
            }
            p.stop();
            blocked = true;
            break;
        }

        // -------------------------------------
        // Vertical movement
        // -------------------------------------

        int otherIndex = occupied[nextY][x];

        if (canDisplace(index, otherIndex))
        {
            // The particle currently at the destination
            // gets pushed into our old position.
            occupied[y][x] = otherIndex;

            if (otherIndex != -1)
            {
                particles[otherIndex].setPosition(x, y);
                activateParticle(otherIndex);

                setPixel(x, y,
                        particles[otherIndex].getMaterial());
            }
            else
            {
                clearPixel(x, y);
                wakeNeighbors(x, y);
            }

            p.setPosition(x, nextY);

            occupied[nextY][x] = index;

            setPixel( x, nextY, p.getMaterial());

            activateParticle(index);

            continue;
        }

        // -------------------------------------
        // Diagonal movement
        // -------------------------------------

        bool moved = false;

        int firstDirection = (fastRandom() & 1) ? -1 : 1;

        for (int attempt = 0; attempt < 2; attempt++)
        {
            int nextX = x + firstDirection;

            if (nextX >= 0 && nextX < WIDTH)
            {
                int diagonalIndex = occupied[nextY][nextX];

                if (canDisplace( index, diagonalIndex))
                {
                    // Move the displaced particle
                    // into our old position.
                    occupied[y][x] = diagonalIndex;

                    if (diagonalIndex != -1)
                    {
                        particles[diagonalIndex] .setPosition( x, y);
                        activateParticle(diagonalIndex);

                        setPixel( x, y, particles[diagonalIndex] .getMaterial());
                    }
                    else
                    {
                        clearPixel(x, y);
                        // The particle that was resting on us
                        // may now be able to fall.
                        wakeNeighbors(x, y);
                    }

                    // Move our particle diagonally
                    p.setPosition( nextX, nextY);
                    // p.setActive(true);
                    activateParticle(index);

                    setPixel( nextX, nextY, p.getMaterial());

                    occupied[nextY][nextX] = index;
                    // wakeNeighbors(nextX, nextY);

                    moved = true;
                    break;
                }
            }

            firstDirection *= -1;
        }

        if (moved)
        {
            activateParticle(index);
            continue;
        }
        // -------------------------------------
        // We couldn't move vertically
        // or diagonally
        // -------------------------------------

        blocked = true;
        break;
    }

    // -----------------------------------------
    // Horizontal spreading
    // -----------------------------------------

    float spread = p.getSpread();

    if (spread > 0.0f)
    {
        int maxSpread = static_cast<int>( spread * 10.0f);

        if (maxSpread < 1)
            maxSpread = 1;

        for (int distance = 1; distance <= maxSpread; distance++)
        {
            int leftX = p.getX() - distance;
            int rightX = p.getX() + distance;

            int y = p.getY();

            if (leftX >= 0 && occupied[y][leftX] == -1)
            {
                int oldX = p.getX();
                int oldY = p.getY();

                // Old cell becomes empty
                occupied[oldY][oldX] = -1;
                clearPixel(oldX, oldY);

                // The particle above may now fall
                wakeNeighbors(oldX, oldY);

                // Move particle
                p.setPosition(leftX, oldY);

                // Occupy new cell
                occupied[oldY][leftX] = index;
                setPixel(leftX, oldY, p.getMaterial());

                // It moved, so process it again next frame
                activateParticle(index);

                break;
            }
            if (rightX < WIDTH && occupied[y][rightX] == -1)
            {
                int oldX = p.getX();
                int oldY = p.getY();

                occupied[oldY][oldX] = -1;
                clearPixel(oldX, oldY);

                wakeNeighbors(oldX, oldY);

                p.setPosition(rightX, oldY);

                occupied[oldY][rightX] = index;
                setPixel(rightX, oldY, p.getMaterial());

                activateParticle(index);

                break;
            }
        }
    }

    // -----------------------------------------
    // Completely blocked
    // -----------------------------------------

    if (blocked && spread <= 0.0f)
    {
        p.stop();
        if (p.hasLifetime())
        {
            activateParticle(index);
        }
    }
}

int main()
{
    clearOccupied();

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

    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT,
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

    Mouse mouse( window, WIDTH, HEIGHT);
    MaterialSelector materialSelector(window);

    glfwMakeContextCurrent(window);


    // --------------------------------------------------
    // 3. Initialize GLAD
    // --------------------------------------------------

    if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    GLuint screenTexture;

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_NEAREST
            );

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_NEAREST
            );

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE
            );

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
            );

    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            WIDTH,
            HEIGHT,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
            );

    // --------------------------------------------------
    // 4. Load shaders from files
    // --------------------------------------------------

    std::string vertexSource = readFile("../shaders/vertex.glsl");

    std::string fragmentSource = readFile("../shaders/fragment.glsl");


    // --------------------------------------------------
    // 5. Compile shaders
    // --------------------------------------------------

    GLuint vertexShader = compileShader( GL_VERTEX_SHADER, vertexSource);

    GLuint fragmentShader = compileShader( GL_FRAGMENT_SHADER, fragmentSource);

    // --------------------------------------------------
    // 6. Create shader program
    // --------------------------------------------------

    GLuint shaderProgram = glCreateProgram();

    glAttachShader( shaderProgram, vertexShader);
    glAttachShader( shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking
    int success;

    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog( shaderProgram, 512, nullptr, infoLog);

        std::cerr << "Shader linking failed:\n" << infoLog << '\n';
    }

    // We don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    int textureLocation = glGetUniformLocation( shaderProgram, "screenTexture");

    glUniform1i( textureLocation, 0);

    // --------------------------------------------------
    // Particle rendering
    // --------------------------------------------------

    float quadVertices[] =
    {
        // Position        // UV
        -1.0f, -1.0f,       0.0f, 0.0f,
        1.0f, -1.0f,       1.0f, 0.0f,
        1.0f,  1.0f,       1.0f, 1.0f,

        -1.0f, -1.0f,       0.0f, 0.0f,
        1.0f,  1.0f,       1.0f, 1.0f,
        -1.0f,  1.0f,       0.0f, 1.0f
    };

    GLuint quadVAO;
    GLuint quadVBO;

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(quadVertices),
            quadVertices,
            GL_STATIC_DRAW
            );
    glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(float),
            (void*)0
            );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(float),
            (void*)(2 * sizeof(float))
            );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

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
            selectedMaterial = newMaterial;
        }

        // ---------------------------------------------
        // Mouse
        // ---------------------------------------------
        mouse.update();

        int gridX = mouse.getX();
        int gridY = mouse.getY();

        const int BRUSH_RADIUS = 8;


        if (mouse.isLeftPressed())
        {
            useBrush( gridX, gridY, BRUSH_RADIUS, false);
        }

        else if (mouse.isErasePressed())
        {
            useBrush( gridX, gridY, BRUSH_RADIUS, true);
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
        // Particle movement
        // ---------------------------------------------

        for (int particleIndex : activeParticles)
        {
            if (particleIndex < 0 || particleIndex >= particleCount)
                continue;

            Particle& p = particles[particleIndex];

            // This particle was scheduled for this frame.
            // Remove its active flag so it can be scheduled
            // again for the NEXT frame.
            p.setActive(false);

            // Particle may already have been removed earlier
            // in this frame.
            if (p.getX() < 0 || p.getY() < 0)
                continue;

            p.updateLifetime(deltaTime);

            if (p.isDead())
            {
                removeParticle(particleIndex);
                continue;
            }
            if (p.hasLifetime())
            {
                activateParticle(particleIndex);
            }

            if (!p.isMovable())
                continue;

            updateParticle(p, particleIndex, deltaTime);
        }

        activeParticles.swap(nextActiveParticles);
        nextActiveParticles.clear();

        // ---------------------------------------------
        // Clear screen
        // ---------------------------------------------

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        // ---------------------------------------------
        // Upload pixel buffer to texture
        // ---------------------------------------------

        glActiveTexture(GL_TEXTURE0);

        glBindTexture( GL_TEXTURE_2D, screenTexture);

        glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                WIDTH,
                HEIGHT,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixelData.data()
                );

        // ---------------------------------------------
        // Draw fullscreen quad
        // ---------------------------------------------

        glUseProgram(shaderProgram);

        glBindVertexArray(quadVAO);

        glDrawArrays( GL_TRIANGLES, 0, 6);

        // ---------------------------------------------
        // Show frame
        // ---------------------------------------------

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
                std::to_string(particleCount) + " | Moving: " +
                std::to_string(activeParticles.size()) + " | FPS: " +
                std::to_string(fps);

            glfwSetWindowTitle( window, title.c_str());

            frameCount = 0;
            fpsTimer = currentFPS;
        }
        glfwPollEvents();
    }

    // --------------------------------------------------
    // 11. Cleanup
    // --------------------------------------------------

    glDeleteVertexArrays( 1, &quadVAO);
    glDeleteBuffers( 1, &quadVBO);
    glDeleteTextures( 1, &screenTexture);

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
