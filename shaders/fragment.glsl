// #version 330 core
//
// in vec4 color;
// out vec4 FragColor;
//
// void main()
// {
//     FragColor = color;
// }
#version 330 core

in vec2 uv;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(screenTexture, uv);
}
