#pragma once


#include <string>
#include <vector>
#include <glad/glad.h>

class Renderer
{
    public:

        Renderer(int width, int height);
        ~Renderer();

        void render( const std::vector<unsigned char>& pixelData);
        void drawCircle(int centerX, int centerY, int radius, float r, float g, float b);
        void drawCircunference( int centerX, int centerY, int radius, float r, float g, float b);
        void createTexture();
        void createQuad();
        void createShaders();
        void createCircle();

        std::string readFile(const char* path);
        GLuint compileShader( GLenum type, const std::string& source);

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
};
