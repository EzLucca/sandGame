#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "Particle.h"
#include "Fire.h"
#include "Materials.h"

const int WIDTH = 800;
const int HEIGHT = 600;

bool occupied[HEIGHT][WIDTH] = {};
const Material* selectedMaterial = &sandMaterial;

const int FIRE_COUNT = 100000;
const int PARTICLE_COUNT = 100000;

Particle particles[PARTICLE_COUNT];

int particleCount = 0;

Fire fires[FIRE_COUNT];
int fireCount = 0;

// Read a text file
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

void placeParticle(int x, int y)
{
    if (x < 0 || x >= WIDTH ||
        y < 0 || y >= HEIGHT)
    {
        return;
    }

    // -----------------------------------------
    // FIRE
    // -----------------------------------------

    if (selectedMaterial->isFire)
    {
        if (fireCount >= FIRE_COUNT)
            return;

        fires[fireCount] = Fire(x, y);

        float upwardVelocity =
            -(400.0f + rand() % 150);

        fires[fireCount].setVelocity(
            upwardVelocity
        );

        float sideVelocity =
            (rand() % 61) - 30.0f;

        fires[fireCount].setHorizontalVelocity(
            sideVelocity
        );

        fireCount++;

        return;
    }

    // -----------------------------------------
    // NORMAL PARTICLE
    // -----------------------------------------

    if (occupied[y][x])
    {
        return;
    }

    if (particleCount >= PARTICLE_COUNT)
    {
        return;
    }

    particles[particleCount] =
        Particle(
            x,
            y,
            *selectedMaterial
        );

    occupied[y][x] = true;

    particleCount++;
}

void createStoneContainer()
{
    int bottomY = 500;
    int leftX = 350;
    int rightX = 450;
    int topY = 450;

    // Left wall
    for (int y = topY; y <= bottomY; y++)
    {
        if (particleCount >= PARTICLE_COUNT)
            return;

        particles[particleCount] =
            Particle(leftX, y, stoneMaterial);

        occupied[y][leftX] = true;

        particleCount++;
    }

    // Right wall
    for (int y = topY; y <= bottomY; y++)
    {
        if (particleCount >= PARTICLE_COUNT)
            return;

        particles[particleCount] =
            Particle(rightX, y, stoneMaterial);

        occupied[y][rightX] = true;

        particleCount++;
    }

    // Bottom
    for (int x = leftX; x <= rightX; x++)
    {
        if (particleCount >= PARTICLE_COUNT)
            return;

        if (occupied[bottomY][x])
            continue;

        particles[particleCount] =
            Particle(x, bottomY, stoneMaterial);

        occupied[bottomY][x] = true;

        particleCount++;
    }
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

        glGetShaderInfoLog(
                shader,
                512,
                nullptr,
                infoLog
                );

        std::cerr << "Shader compilation failed:\n"
            << infoLog << '\n';
    }

    return shader;
}


int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
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
    glfwWindowHint(
            GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE
            );


    // --------------------------------------------------
    // 2. Create window
    // --------------------------------------------------

    GLFWwindow* window = glfwCreateWindow(
            WIDTH,
            HEIGHT,
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

    glfwMakeContextCurrent(window);


    // --------------------------------------------------
    // 3. Initialize GLAD
    // --------------------------------------------------

    if (!gladLoadGLLoader(
                (GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }


    // --------------------------------------------------
    // 4. Load shaders from files
    // --------------------------------------------------

    std::string vertexSource =
        readFile("../shaders/vertex.glsl");

    std::string fragmentSource =
        readFile("../shaders/fragment.glsl");


    // --------------------------------------------------
    // 5. Compile shaders
    // --------------------------------------------------

    GLuint vertexShader =
        compileShader(
                GL_VERTEX_SHADER,
                vertexSource
                );

    GLuint fragmentShader =
        compileShader(
                GL_FRAGMENT_SHADER,
                fragmentSource
                );


    // --------------------------------------------------
    // 6. Create shader program
    // --------------------------------------------------

    GLuint shaderProgram =
        glCreateProgram();

    glAttachShader(
            shaderProgram,
            vertexShader
            );

    glAttachShader(
            shaderProgram,
            fragmentShader
            );

    glLinkProgram(shaderProgram);


    // Check linking
    int success;

    glGetProgramiv(
            shaderProgram,
            GL_LINK_STATUS,
            &success
            );

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(
                shaderProgram,
                512,
                nullptr,
                infoLog
                );

        std::cerr << "Shader linking failed:\n"
            << infoLog << '\n';
    }


    // We don't need these anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // --------------------------------------------------
    // 7. Create our vertex
    // --------------------------------------------------

    float vertex[] =
    {
        0.0f,
        0.0f
    };


    // --------------------------------------------------
    // 8. Create VBO
    // --------------------------------------------------

    GLuint VBO;

    glGenBuffers(1, &VBO);

    glBindBuffer(
            GL_ARRAY_BUFFER,
            VBO
            );

    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertex),
            vertex,
            GL_STATIC_DRAW
            );


    // --------------------------------------------------
    // 9. Create VAO
    // --------------------------------------------------

    GLuint VAO;

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(
            GL_ARRAY_BUFFER,
            VBO
            );

    glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            2 * sizeof(float),
            (void*)0
            );

    glEnableVertexAttribArray(0);

    // --------------------------------------------------
    // Particle arrays
    // --------------------------------------------------
    // Particle particles[PARTICLE_COUNT];
    //
    // int sandCount = 0;



    // --------------------------------------------------
    // Timing 
    // --------------------------------------------------

    float spawnTimer = 0.0f;

    float fireSpawnTimer = 0.0f;

    float lastTime = glfwGetTime();

    const float gravity = 500.0f;
    const float fireGravity = -200.0f;


    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    spawnTimer += deltaTime;


    int positionLocation =
        glGetUniformLocation(shaderProgram, "position");
    int windowSizeLocation =
        glGetUniformLocation(shaderProgram, "windowSize");
    int colorLocation =
        glGetUniformLocation(shaderProgram, "color");

    createStoneContainer();

    // --------------------------------------------------
    // 10. Main loop
    // --------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // ---------------------------------------------
        // Material selection
        // ---------------------------------------------

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            selectedMaterial = &sandMaterial;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            selectedMaterial = &stoneMaterial;
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            selectedMaterial = &fireMaterial;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        // ---------------------------------------------
        // Mouse
        // ---------------------------------------------

        double mouseX;
        double mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth;
        int windowHeight;

        glfwGetWindowSize(
                window,
                &windowWidth,
                &windowHeight
                );

        int gridX =
            static_cast<int>(
                    mouseX * WIDTH / windowWidth
                    );

        int gridY =
            static_cast<int>(
                    mouseY * HEIGHT / windowHeight
                    );
        const int BRUSH_RADIUS = 4;

        if (glfwGetMouseButton(
                    window,
                    GLFW_MOUSE_BUTTON_LEFT
                    ) == GLFW_PRESS)
        {
            for (int dx = -BRUSH_RADIUS; dx <= BRUSH_RADIUS; dx++)
            {
                for (int dy = -BRUSH_RADIUS; dy <= BRUSH_RADIUS; dy++)
                {
                    if (dx * dx + dy * dy <=
                            BRUSH_RADIUS * BRUSH_RADIUS)
                    {
                        placeParticle(
                                gridX + dx,
                                gridY + dy
                                );
                    }
                }
            }
        }

        // ---------------------------------------------
        // Time
        // ---------------------------------------------

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // ---------------------------------------------
        // Spawn a new particle every second
        // ---------------------------------------------
        spawnTimer += deltaTime;

        if (spawnTimer >= 0.2f &&
                particleCount < PARTICLE_COUNT)
        {
            int spawnX = 400;
            int spawnY = 0;

            if (!occupied[spawnY][spawnX])
            {
                particles[particleCount] =
                    Particle(
                            spawnX,
                            spawnY,
                            sandMaterial
                            );

                occupied[spawnY][spawnX] = true;

                particleCount++;
            }

            spawnTimer = 0.0f;
        }

        // ---------------------------------------------
        // Fire spawning
        // ---------------------------------------------

        fireSpawnTimer += deltaTime;

        if (fireSpawnTimer >= 0.1f &&
                fireCount < FIRE_COUNT)
        {
            int fireSpawnX = 200;
            int fireSpawnY = 500;

            fires[fireCount] = Fire( fireSpawnX, fireSpawnY);

            float upwardVelocity = -(400.0f + rand() % 150);
            fires[fireCount].setVelocity(upwardVelocity);
            float sideVelocity = -(30.0f + rand() % 61);
            fires[fireCount].setHorizontalVelocity(sideVelocity);
            fireCount++;

            fireSpawnTimer = 0.0f;
        }

        // ---------------------------------------------
        // Particle movement
        // ---------------------------------------------

        for (int i = 0; i < particleCount; i++)
        {
            if (!particles[i].isMovable())
            {
                continue;
            }
            int x = particles[i].getX();
            int y = particles[i].getY();

            // Try down
            // Gravity
            if (particles[i].isAffectedByGravity())
            {
                particles[i].applyGravity(gravity, deltaTime);
            }

            // Movement
            float movement = particles[i].getVelocity() * deltaTime;

            if (movement >= 1.0f)
            {
                int steps = static_cast<int>(movement);

                for (int step = 0; step < steps; step++)
                {
                    x = particles[i].getX();
                    y = particles[i].getY();

                    // -----------------------------------------
                    // Try down
                    // -----------------------------------------

                    if (y + 1 < HEIGHT &&
                            !occupied[y + 1][x])
                    {
                        occupied[y][x] = false;

                        particles[i].moveDown();

                        occupied[
                            particles[i].getY()
                        ][
                        particles[i].getX()
                        ] = true;

                        continue;
                    }

                    // -----------------------------------------
                    // Try down-left
                    // -----------------------------------------

                    if (y + 1 < HEIGHT &&
                            x - 1 >= 0 &&
                            !occupied[y + 1][x - 1])
                    {
                        occupied[y][x] = false;

                        particles[i].moveDownLeft();

                        occupied[
                            particles[i].getY()
                        ][
                        particles[i].getX()
                        ] = true;

                        continue;
                    }

                    // -----------------------------------------
                    // Try down-right
                    // -----------------------------------------

                    if (y + 1 < HEIGHT &&
                            x + 1 < WIDTH &&
                            !occupied[y + 1][x + 1])
                    {
                        occupied[y][x] = false;

                        particles[i].moveDownRight();

                        occupied[
                            particles[i].getY()
                        ][
                        particles[i].getX()
                        ] = true;

                        continue;
                    }

                    // -----------------------------------------
                    // Completely blocked
                    // -----------------------------------------

                    particles[i].stop();
                    break;
                }
            } 
        }

        // ---------------------------------------------
        // Update fire
        // ---------------------------------------------

        for (int i = 0; i < fireCount; i++)
        {
            fires[i].update(fireGravity, deltaTime);
        }

        // ---------------------------------------------
        // Clear screen
        // ---------------------------------------------

        glClearColor(
                0.0f,
                0.0f,
                0.0f,
                1.0f
                );

        glClear(GL_COLOR_BUFFER_BIT);


        // ---------------------------------------------
        // Draw
        // ---------------------------------------------

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        glUniform2f( windowSizeLocation,
                static_cast<float>(WIDTH),
                static_cast<float>(HEIGHT));
        // ---------------------------------------------
        // Draw stone
        // ---------------------------------------------

        for (int i = 0; i < particleCount; i++)
        {
            const Material& material =
                particles[i].getMaterial();

            glUniform4f(
                    colorLocation,
                    material.r,
                    material.g,
                    material.b,
                    material.a
                    );

            glUniform2f(
                    positionLocation,
                    static_cast<float>(
                        particles[i].getX()
                        ),
                    static_cast<float>(
                        particles[i].getY()
                        )
                    );

            glDrawArrays(GL_POINTS, 0, 1);
        }

        // Draw fire

        glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < fireCount; i++)
        {
            if (!fires[i].isDead())
            {
                glUniform2f(
                        positionLocation,
                        static_cast<float>(
                            fires[i].getX()
                            ),
                        static_cast<float>(
                            fires[i].getY()
                            )
                        );

                glDrawArrays(
                        GL_POINTS,
                        0,
                        1
                        );
            }
        }


        // ---------------------------------------------
        // Show frame
        // ---------------------------------------------

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // --------------------------------------------------
    // 11. Cleanup
    // --------------------------------------------------

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
