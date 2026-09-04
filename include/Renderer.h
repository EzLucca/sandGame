#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

class Renderer
{
    public:

        Renderer(int width, int height);
        ~Renderer();

        void render( const std::vector<unsigned char>& pixelData);
        void drawCircle(int centerX, int centerY, int radius, float r, float g, float b);

    private:

        int width;
        int height;

        GLuint screenTexture = 0;

        GLuint shaderProgram = 0;

        GLuint quadVAO = 0;
        GLuint quadVBO = 0;

        // Explosion circle
        GLuint circleShaderProgram = 0;
        GLuint circleVAO = 0;
        GLuint circleVBO = 0;

        void createTexture();
        void createQuad();
        void createShaders();
        void createCircle();

        std::string readFile(const char* path);

        GLuint compileShader(
                GLenum type,
                const std::string& source
                );
};
