#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const int WIDTH = 800;
const int HEIGHT = 600;

bool occupied[HEIGHT][WIDTH] = {};

struct Particle
{
    int x;
    int y;
};

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
            800,
            600,
            "One Pixel",
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

    const int PARTICLE_COUNT = 100;
    Particle particles[PARTICLE_COUNT];
    int particleCount = 0;
    float spawnTimer = 0.0f;
    float lastTime = glfwGetTime();

    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    spawnTimer += deltaTime;

    if (spawnTimer >= 1.0f &&
            particleCount < PARTICLE_COUNT)
    {
        particles[particleCount].x = 400;
        particles[particleCount].y = 0;

        occupied[0][400] = true;

        particleCount++;

        spawnTimer = 0.0f;
    }

    float velocity = 0.0f;

    float gravity = 500.0f;


    int positionLocation =
        glGetUniformLocation(shaderProgram, "position");
    std::cout << "position location: "
        << positionLocation << '\n';
    int windowSizeLocation =
        glGetUniformLocation(shaderProgram, "windowSize");
    std::cout << "windowSize location: "
        << windowSizeLocation << '\n';
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

    spawnTimer += deltaTime;


    // ---------------------------------------------
    // Spawn a new particle every second
    // ---------------------------------------------

    if (spawnTimer >= 1.0f &&
        particleCount < PARTICLE_COUNT)
    {
        int spawnX = 400;
        int spawnY = 0;

        // Only spawn if the location is free
        if (!occupied[spawnY][spawnX])
        {
            particles[particleCount].x = spawnX;
            particles[particleCount].y = spawnY;

            occupied[spawnY][spawnX] = true;

            particleCount++;
        }

        spawnTimer = 0.0f;
    }


    // ---------------------------------------------
    // Particle movement
    // ---------------------------------------------

    for (int i = 0; i < particleCount; i++)
    {
        int x = particles[i].x;
        int y = particles[i].y;

        // Try down
        if (y + 1 < HEIGHT &&
            !occupied[y + 1][x])
        {
            occupied[y][x] = false;

            particles[i].y++;

            occupied[particles[i].y][x] = true;
        }

        // Try down-left
        else if (y + 1 < HEIGHT &&
                 x - 1 >= 0 &&
                 !occupied[y + 1][x - 1])
        {
            occupied[y][x] = false;

            particles[i].x--;
            particles[i].y++;

            occupied[particles[i].y][particles[i].x] = true;
        }

        // Try down-right
        else if (y + 1 < HEIGHT &&
                 x + 1 < WIDTH &&
                 !occupied[y + 1][x + 1])
        {
            occupied[y][x] = false;

            particles[i].x++;
            particles[i].y++;

            occupied[particles[i].y][particles[i].x] = true;
        }
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

        for (int i = 0; i < PARTICLE_COUNT; i++)
        {
            glUniform2f(
                    positionLocation,
                    static_cast<float>(particles[i].x),
                    static_cast<float>(particles[i].y)
                    );

            glDrawArrays(GL_POINTS, 0, 1);
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
