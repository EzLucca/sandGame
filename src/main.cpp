#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>

#include "Particle.h"
#include "Fire.h"
#include "Materials.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int occupied[HEIGHT][WIDTH];

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

    pixelData[index + 0] =
        static_cast<unsigned char>(material.r * 255.0f);

    pixelData[index + 1] =
        static_cast<unsigned char>(material.g * 255.0f);

    pixelData[index + 2] =
        static_cast<unsigned char>(material.b * 255.0f);

    pixelData[index + 3] =
        static_cast<unsigned char>(material.a * 255.0f);
}

inline void clearPixel(int x, int y)
{
    if (x < 0 || x >= WIDTH ||
            y < 0 || y >= HEIGHT)
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

const Material* selectedMaterial = &sandMaterial;

const int PARTICLE_COUNT = 1000000;

Particle particles[PARTICLE_COUNT];

int particleCount = 0;

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

// void placeParticle(int x, int y)
// {
//     if (x < 0 || x >= WIDTH ||
//             y < 0 || y >= HEIGHT)
//         return;
//
//     if (particleCount >= PARTICLE_COUNT)
//         return;
//
//     if (occupied[y][x] != -1)
//         return;
//
//     particles[particleCount] =
//         Particle(
//                 x,
//                 y,
//                 *selectedMaterial
//                 );
//
//     // Fire gets an initial upward velocity
//     if (selectedMaterial->isFire)
//     {
//         particles[particleCount].setVelocity(
//                 -(400.0f + rand() % 150)
//                 );
//
//         particles[particleCount].setHorizontalVelocity(
//                 (rand() % 61) - 30.0f
//                 );
//         particles[particleCount].setLifetime(
//                 1.0f + (rand() % 100) / 100.0f
//                 );
//     }
//
//     occupied[y][x] = particleCount;
//
//     particleCount++;
// }

void placeParticle(int x, int y)
{
    if (x < 0 || x >= WIDTH ||
            y < 0 || y >= HEIGHT)
        return;

    if (particleCount >= PARTICLE_COUNT)
        return;

    if (occupied[y][x] != -1)
        return;

    particles[particleCount] =
        Particle(
                x,
                y,
                *selectedMaterial
                );

    // Fire
    if (selectedMaterial->isFire)
    {
        particles[particleCount].setVelocity(
                -(400.0f + fastRandom() % 150)
                );

        particles[particleCount].setHorizontalVelocity(
                static_cast<float>(
                    static_cast<int>(fastRandom() % 61) - 30
                    )
                );

        particles[particleCount].setLifetime(
                1.0f + (fastRandom() % 100) / 100.0f
                );
    }

    occupied[y][x] = particleCount;

    // NEW:
    // Draw the particle directly into the pixel buffer.
    setPixel(
            x,
            y,
            *selectedMaterial
            );

    particleCount++;
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

// void removeParticle(int index)
// {
//     if (index < 0 || index >= particleCount)
//         return;
//
//     int lastIndex = particleCount - 1;
//
//     int x = particles[index].getX();
//     int y = particles[index].getY();
//
//     // Remove the particle from the occupied grid
//     occupied[y][x] = -1;
//
//     // If this isn't the last particle,
//     // move the last particle into its place
//     if (index != lastIndex)
//     {
//         int lastX = particles[lastIndex].getX();
//         int lastY = particles[lastIndex].getY();
//
//         // Remove old occupied position
//         occupied[lastY][lastX] = -1;
//
//         // Move last particle into removed particle's slot
//         particles[index] = particles[lastIndex];
//
//         // Update occupied grid
//         int newX = particles[index].getX();
//         int newY = particles[index].getY();
//
//         occupied[newY][newX] = index;
//     }
//
//     particleCount--;
// }

void removeParticle(int index)
{
    if (index < 0 || index >= particleCount)
        return;

    int lastIndex = particleCount - 1;

    int x = particles[index].getX();
    int y = particles[index].getY();

    // Remove particle from grid
    occupied[y][x] = -1;

    // Remove its pixel
    clearPixel(x, y);

    if (index != lastIndex)
    {
        int lastX =
            particles[lastIndex].getX();

        int lastY =
            particles[lastIndex].getY();

        // Remove old grid position
        occupied[lastY][lastX] = -1;

        // Move last particle into this slot
        particles[index] =
            particles[lastIndex];

        // Update grid
        int newX =
            particles[index].getX();

        int newY =
            particles[index].getY();

        occupied[newY][newX] = index;

        // Make sure the moved particle is rendered
        setPixel(
                newX,
                newY,
                particles[index].getMaterial()
                );
    }

    particleCount--;
}

// void updateParticle(
//         Particle& p,
//         int index,
//         float deltaTime)
// {
//     // -----------------------------------------
//     // Gravity
//     // -----------------------------------------
//
//     if (p.isAffectedByGravity())
//     {
//         p.applyGravity(
//                 p.getMaterial().gravityValue,
//                 deltaTime
//                 );
//     }
//
//     // -----------------------------------------
//     // Calculate vertical movement
//     // -----------------------------------------
//
//     float movement =
//         p.getVelocity() * deltaTime;
//
//     int steps =
//         static_cast<int>(std::abs(movement));
//
//     steps = std::clamp(steps, 1, 4);
//
//     int direction =
//         p.getMaterial().dir;
//
//     // -----------------------------------------
//     // Vertical + diagonal movement
//     // -----------------------------------------
//
//     bool blocked = false;
//
//     for (int step = 0; step < steps; step++)
//     {
//         int x = p.getX();
//         int y = p.getY();
//
//         int nextY = y + direction;
//
//         // -------------------------------------
//         // Outside screen
//         // -------------------------------------
//
//         if (nextY < 0 || nextY >= HEIGHT)
//         {
//             p.stop();
//             blocked = true;
//             break;
//         }
//
//         // -------------------------------------
//         // Vertical movement
//         // -------------------------------------
//
//         int otherIndex =
//             occupied[nextY][x];
//
//         if (canDisplace(index, otherIndex))
//         {
//             clearPixel(x, y);
//             occupied[y][x] = otherIndex;
//
//             if (otherIndex != -1)
//             {
//                 particles[otherIndex].setPosition(x, y);
//                 setPixel( x, y,
//                         particles[otherIndex].getMaterial()
//                         );
//             }
//
//             p.setPosition(x, nextY);
//
//             occupied[nextY][x] = index;
//             // Draw particle at new position
//             setPixel(
//                     x,
//                     nextY,
//                     p.getMaterial()
//                     );
//
//             continue;
//         }
//
//         // -------------------------------------
//         // Diagonal movement
//         // -------------------------------------
//
//         bool moved = false;
//
//         int firstDirection =
//             (fastRandom() & 1) ? -1 : 1;
//
//         for (int attempt = 0; attempt < 2; attempt++)
//         {
//             int nextX = x + firstDirection;
//
//             if (nextX >= 0 && nextX < WIDTH)
//             {
//                 int diagonalIndex =
//                     occupied[nextY][nextX];
//
//                 if (canDisplace(index, diagonalIndex))
//                 {
//                     occupied[y][x] = diagonalIndex;
//
//                     if (diagonalIndex != -1)
//                     {
//                         particles[diagonalIndex]
//                             .setPosition(x, y);
//                     }
//
//                     p.setPosition(
//                             nextX,
//                             nextY
//                             );
//
//                     occupied[nextY][nextX] = index;
//
//                     moved = true;
//                     break;
//                 }
//             }
//
//             firstDirection *= -1;
//         }
//
//         if (moved)
//             continue;
//
//         // We couldn't move vertically or diagonally
//         blocked = true;
//         break;
//     }
//
//     // -----------------------------------------
//     // Horizontal spreading
//     // -----------------------------------------
//
//     float spread = p.getSpread();
//
//     if (spread > 0.0f)
//     {
//         int maxSpread =
//             static_cast<int>(spread * 10.0f);
//
//         if (maxSpread < 1)
//             maxSpread = 1;
//
//         for (int distance = 0;
//                 distance < maxSpread;
//                 distance++)
//         {
//             int x = p.getX();
//             int y = p.getY();
//
//             int side =
//                 (fastRandom() & 1)
//                 ? -1
//                 : 1;
//
//             int nextX = x + side;
//
//             // Try chosen direction
//             if (nextX >= 0 &&
//                     nextX < WIDTH &&
//                     occupied[y][nextX] == -1)
//             {
//                 occupied[y][x] = -1;
//
//                 p.setPosition(
//                         nextX,
//                         y
//                         );
//
//                 occupied[y][nextX] = index;
//
//                 continue;
//             }
//
//             // Try opposite direction
//             nextX = x - side;
//
//             if (nextX >= 0 &&
//                     nextX < WIDTH &&
//                     occupied[y][nextX] == -1)
//             {
//                 occupied[y][x] = -1;
//
//                 p.setPosition(
//                         nextX,
//                         y
//                         );
//
//                 occupied[y][nextX] = index;
//
//                 continue;
//             }
//
//             break;
//         }
//     }
//
//     // -----------------------------------------
//     // Completely blocked
//     // -----------------------------------------
//
//     if (blocked && spread <= 0.0f)
//     {
//         p.stop();
//     }
// }
void updateParticle(
        Particle& p,
        int index,
        float deltaTime)
{
    // -----------------------------------------
    // Gravity
    // -----------------------------------------

    if (p.isAffectedByGravity())
    {
        p.applyGravity(
                p.getMaterial().gravityValue,
                deltaTime
                );
    }

    // -----------------------------------------
    // Calculate vertical movement
    // -----------------------------------------

    float movement =
        p.getVelocity() * deltaTime;

    int steps =
        static_cast<int>(std::abs(movement));

    steps = std::clamp(steps, 1, 4);

    int direction =
        p.getMaterial().dir;

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
            p.stop();
            blocked = true;
            break;
        }

        // -------------------------------------
        // Vertical movement
        // -------------------------------------

        int otherIndex =
            occupied[nextY][x];

        if (canDisplace(index, otherIndex))
        {
            // The particle currently at the destination
            // gets pushed into our old position.
            occupied[y][x] = otherIndex;

            if (otherIndex != -1)
            {
                particles[otherIndex].setPosition(
                        x,
                        y
                        );

                // Draw displaced particle
                setPixel(
                        x,
                        y,
                        particles[otherIndex].getMaterial()
                        );
            }
            else
            {
                // Old position becomes empty
                clearPixel(x, y);
            }

            // Move our particle
            p.setPosition(
                    x,
                    nextY
                    );

            // Draw our particle at its new position
            setPixel(
                    x,
                    nextY,
                    p.getMaterial()
                    );

            occupied[nextY][x] = index;

            continue;
        }

        // -------------------------------------
        // Diagonal movement
        // -------------------------------------

        bool moved = false;

        int firstDirection =
            (fastRandom() & 1) ? -1 : 1;

        for (int attempt = 0; attempt < 2; attempt++)
        {
            int nextX =
                x + firstDirection;

            if (nextX >= 0 &&
                    nextX < WIDTH)
            {
                int diagonalIndex =
                    occupied[nextY][nextX];

                if (canDisplace(
                            index,
                            diagonalIndex))
                {
                    // Move the displaced particle
                    // into our old position.
                    occupied[y][x] =
                        diagonalIndex;

                    if (diagonalIndex != -1)
                    {
                        particles[diagonalIndex]
                            .setPosition(
                                    x,
                                    y
                                    );

                        setPixel(
                                x,
                                y,
                                particles[diagonalIndex]
                                .getMaterial()
                                );
                    }
                    else
                    {
                        clearPixel(x, y);
                    }

                    // Move our particle diagonally
                    p.setPosition(
                            nextX,
                            nextY
                            );

                    setPixel(
                            nextX,
                            nextY,
                            p.getMaterial()
                            );

                    occupied[nextY][nextX] =
                        index;

                    moved = true;
                    break;
                }
            }

            firstDirection *= -1;
        }

        if (moved)
            continue;

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

    float spread =
        p.getSpread();

    if (spread > 0.0f)
    {
        int maxSpread =
            static_cast<int>(
                    spread * 10.0f
                    );

        if (maxSpread < 1)
            maxSpread = 1;

        for (int distance = 0;
                distance < maxSpread;
                distance++)
        {
            int x = p.getX();
            int y = p.getY();

            int side =
                (fastRandom() & 1)
                ? -1
                : 1;

            int nextX =
                x + side;

            // ---------------------------------
            // Try chosen direction
            // ---------------------------------

            if (nextX >= 0 &&
                    nextX < WIDTH &&
                    occupied[y][nextX] == -1)
            {
                // Clear old pixel
                clearPixel(x, y);

                occupied[y][x] = -1;

                // Move particle
                p.setPosition(
                        nextX,
                        y
                        );

                // Draw new pixel
                setPixel(
                        nextX,
                        y,
                        p.getMaterial()
                        );

                occupied[y][nextX] = index;

                continue;
            }

            // ---------------------------------
            // Try opposite direction
            // ---------------------------------

            nextX =
                x - side;

            if (nextX >= 0 &&
                    nextX < WIDTH &&
                    occupied[y][nextX] == -1)
            {
                // Clear old pixel
                clearPixel(x, y);

                occupied[y][x] = -1;

                // Move particle
                p.setPosition(
                        nextX,
                        y
                        );

                // Draw new pixel
                setPixel(
                        nextX,
                        y,
                        p.getMaterial()
                        );

                occupied[y][nextX] = index;

                continue;
            }

            // Neither direction worked
            break;
        }
    }

    // -----------------------------------------
    // Completely blocked
    // -----------------------------------------

    if (blocked && spread <= 0.0f)
    {
        p.stop();
    }
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
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

    glUseProgram(shaderProgram);

    int textureLocation =
        glGetUniformLocation(
                shaderProgram,
                "screenTexture"
                );

    glUniform1i(
            textureLocation,
            0
            );

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

    // // --------------------------------------------------
    // // Particle rendering VAO + VBO
    // // --------------------------------------------------
    //
    // GLuint VAO;
    // GLuint particleVBO;
    //
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &particleVBO);
    //
    // glBindVertexArray(VAO);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    //
    // glBufferData(
    //         GL_ARRAY_BUFFER,
    //         PARTICLE_COUNT * 6 * sizeof(float),
    //         nullptr,
    //         GL_DYNAMIC_DRAW
    //         );
    //
    // // ---------------------------------------------
    // // Position: x, y
    // // ---------------------------------------------
    //
    // glVertexAttribPointer(
    //         0,
    //         2,
    //         GL_FLOAT,
    //         GL_FALSE,
    //         6 * sizeof(float),
    //         (void*)0
    //         );
    //
    // glEnableVertexAttribArray(0);
    //
    // // ---------------------------------------------
    // // Color: r, g, b, a
    // // ---------------------------------------------
    //
    // glVertexAttribPointer(
    //         1,
    //         4,
    //         GL_FLOAT,
    //         GL_FALSE,
    //         6 * sizeof(float),
    //         (void*)(2 * sizeof(float))
    //         );
    //
    // glEnableVertexAttribArray(1);
    //
    // glBindVertexArray(0);

    // --------------------------------------------------
    // Particle arrays
    // --------------------------------------------------

    std::vector<float> renderData;

    renderData.reserve(PARTICLE_COUNT * 6);

    // int windowSizeLocation =
    //     glGetUniformLocation(shaderProgram, "windowSize");

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

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            selectedMaterial = &waterMaterial;
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            selectedMaterial = &smokeMaterial;
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

        const int BRUSH_RADIUS = 8;

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

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            for (int dx = -BRUSH_RADIUS; dx <= BRUSH_RADIUS; dx++)
            {
                for (int dy = -BRUSH_RADIUS; dy <= BRUSH_RADIUS; dy++)
                {
                    if (dx * dx + dy * dy <=
                            BRUSH_RADIUS * BRUSH_RADIUS)
                    {
                        int x = gridX + dx;
                        int y = gridY + dy;

                        // Check bounds
                        if (x < 0 || x >= WIDTH ||
                                y < 0 || y >= HEIGHT)
                        {
                            continue;
                        }

                        // Get particle index from grid
                        int index = occupied[y][x];

                        if (index != -1)
                        {
                            removeParticle(index);
                        }
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
        // Particle movement
        // ---------------------------------------------

        for (int i = 0; i < particleCount; i++)
        {
            Particle& p = particles[i];

            // Lifetime
            p.updateLifetime(deltaTime);

            if (p.isDead())
            {
                removeParticle(i);
                i--;
                continue;
            }

            // Movement
            if (!p.isMovable())
                continue;

            updateParticle(
                    p,
                    i,
                    deltaTime
                    );
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

        // glUseProgram(shaderProgram);
        //
        // glBindVertexArray(VAO);
        //
        // glUniform2f( windowSizeLocation,
        //         static_cast<float>(WIDTH),
        //         static_cast<float>(HEIGHT));
        //
        // // ---------------------------------------------
        // // Draw particle
        // // ---------------------------------------------
        // renderData.clear();
        //
        // for (int i = 0; i < particleCount; i++)
        // {
        //     const Particle& p = particles[i];
        //     const Material& m = p.getMaterial();
        //
        //     int j = i * 6;
        //
        //     renderData[j + 0] = static_cast<float>(p.getX());
        //     renderData[j + 1] = static_cast<float>(p.getY());
        //
        //     renderData[j + 2] = m.r;
        //     renderData[j + 3] = m.g;
        //     renderData[j + 4] = m.b;
        //     renderData[j + 5] = m.a;
        // }
        //
        // // Upload particle data
        // glBindBuffer( GL_ARRAY_BUFFER, particleVBO);
        //
        // glBufferSubData(
        //         GL_ARRAY_BUFFER,
        //         0,
        //         particleCount * 6 * sizeof(float),
        //         renderData.data()
        //         );
        //
        // // glBufferSubData(
        // //         GL_ARRAY_BUFFER,
        // //         0,
        // //         renderData.size() * sizeof(float),
        // //         renderData.data()
        // //         );
        //
        // glUseProgram(shaderProgram);
        //
        // // Draw ALL particles in one call
        // glBindVertexArray(VAO);
        //
        // glUniform2f(
        //         windowSizeLocation,
        //         static_cast<float>(WIDTH),
        //         static_cast<float>(HEIGHT)
        //         );
        //
        // glDrawArrays(
        //         GL_POINTS,
        //         0,
        //         particleCount
        //         );

        // ---------------------------------------------
        // Upload pixel buffer to texture
        // ---------------------------------------------

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
                GL_TEXTURE_2D,
                screenTexture
                );

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
        int textureLocation =
            glGetUniformLocation(
                    shaderProgram,
                    "screenTexture"
                    );

        glUniform1i(
                textureLocation,
                0
                );

        glBindVertexArray(quadVAO);

        glDrawArrays(
                GL_TRIANGLES,
                0,
                6
                );

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
            double fps =
                frameCount / (currentFPS - fpsTimer);

            std::string title =
                "Pixel simulation | Particles: " +
                std::to_string(particleCount) +
                " | FPS: " +
                std::to_string(fps);

            glfwSetWindowTitle(
                    window,
                    title.c_str()
                    );

            frameCount = 0;
            fpsTimer = currentFPS;
        }
        glfwPollEvents();
    }

    // --------------------------------------------------
    // 11. Cleanup
    // --------------------------------------------------

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &particleVBO);

    glDeleteVertexArrays( 1, &quadVAO);
    glDeleteBuffers( 1, &quadVBO);
    glDeleteTextures( 1, &screenTexture);

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
