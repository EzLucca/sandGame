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

const int WIDTH = 800;
const int HEIGHT = 600;

bool occupied[HEIGHT][WIDTH] = {};

const int PARTICLE_COUNT = 10000;
const int FIRE_COUNT = 10000;


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
    Particle particles[PARTICLE_COUNT];

    int particleCount = 0;

    Fire fires[FIRE_COUNT];

    int fireCount = 0;

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
    // --------------------------------------------------
    // 10. Main loop
    // --------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
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

            // Only spawn if the location is free
            if (!occupied[spawnY][spawnX])
            {
                particles[particleCount] = Particle(spawnX, spawnY);

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

            float upwardVelocity = -(100.0f + rand() % 150);
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
            particles[i].applyGravity(gravity, deltaTime);
            int x = particles[i].getX();
            int y = particles[i].getY();

            // Try down
            // Gravity
            particles[i].applyGravity(gravity, deltaTime);

            // Movement
            float movement = particles[i].getVelocity() * deltaTime;

            if (movement >= 1.0f)
            {
                int steps = static_cast<int>(movement);

                for (int step = 0; step < steps; step++)
                {
                    int x = particles[i].getX();
                    int y = particles[i].getY();

                    if (y + 1 < HEIGHT &&
                            !occupied[y + 1][x])
                    {
                        occupied[y][x] = false;

                        particles[i].moveDown();

                        occupied[particles[i].getY()][particles[i].getX()] = true;
                    }
                    else
                    {
                        particles[i].stop();
                        break;
                    }
                }
            }
            // Try down-left
            else if (y + 1 < HEIGHT &&
                    x - 1 >= 0 &&
                    !occupied[y + 1][x - 1])
            {
                occupied[y][x] = false;

                particles[i].moveLeft();

                occupied[particles[i].getY()][particles[i].getX()] = true;
            }

            // Try down-right
            else if (y + 1 < HEIGHT &&
                    x + 1 < WIDTH &&
                    !occupied[y + 1][x + 1])
            {
                occupied[y][x] = false;

                particles[i].moveRight();

                occupied[particles[i].getY()][particles[i].getX()] = true;
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

        for (int i = 0; i < particleCount; i++)
        {
            glUniform2f(
                    positionLocation,
                    static_cast<float>(particles[i].getX()),
                    static_cast<float>(particles[i].getY())
                    );

            glDrawArrays(GL_POINTS, 0, 1);
        }
        // Draw fire

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
        glUniform2f( windowSizeLocation, 800.0f, 600.0f);


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
