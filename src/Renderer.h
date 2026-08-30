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

    private:

        int width;
        int height;

        GLuint screenTexture = 0;

        GLuint shaderProgram = 0;

        GLuint quadVAO = 0;
        GLuint quadVBO = 0;

        void createTexture();
        void createQuad();
        void createShaders();

        std::string readFile(const char* path);

        GLuint compileShader(
                GLenum type,
                const std::string& source
                );
};
