#include "Renderer.h"

#include <fstream>
#include <iostream>
#include <sstream>

Renderer::Renderer(int width, int height)
    : width(width),
      height(height)
{
    createTexture();
    createShaders();
    createQuad();
}

Renderer::~Renderer()
{
    if (quadVAO != 0)
        glDeleteVertexArrays(1, &quadVAO);

    if (quadVBO != 0)
        glDeleteBuffers(1, &quadVBO);

    if (screenTexture != 0)
        glDeleteTextures(1, &screenTexture);

    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
}

void Renderer::createTexture()
{
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
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindTexture(GL_TEXTURE_2D, 0);
}

std::string Renderer::readFile(const char* path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Could not open: "
                  << path << '\n';

        return "";
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}
GLuint Renderer::compileShader(
    GLenum type,
    const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* src = source.c_str();

    glShaderSource(
        shader,
        1,
        &src,
        nullptr
    );

    glCompileShader(shader);

    int success;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader compilation failed:\n"
            << infoLog
            << '\n';
    }

    return shader;
}

void Renderer::createShaders()
{
    std::string vertexSource =
        readFile("../shaders/vertex.glsl");

    std::string fragmentSource =
        readFile("../shaders/fragment.glsl");

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

    shaderProgram = glCreateProgram();

    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );

    glLinkProgram(shaderProgram);

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

        std::cerr
            << "Shader linking failed:\n"
            << infoLog
            << '\n';
    }

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
}
void Renderer::createQuad()
{
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

    glGenVertexArrays(
        1,
        &quadVAO
    );

    glGenBuffers(
        1,
        &quadVBO
    );

    glBindVertexArray(quadVAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        quadVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(quadVertices),
        quadVertices,
        GL_STATIC_DRAW
    );

    // Position
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // UV
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
}

void Renderer::render(
    const std::vector<unsigned char>& pixelData)
{
    glClearColor(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);

    // Upload simulation pixels
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
        width,
        height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixelData.data()
    );

    // Draw texture
    glUseProgram(shaderProgram);

    glBindVertexArray(quadVAO);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );

    glBindVertexArray(0);
}
