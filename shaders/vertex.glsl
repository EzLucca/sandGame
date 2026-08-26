// #version 330 core
//
// layout(location = 0) in vec2 position;
// layout(location = 1) in vec4 particleColor;
//
// uniform vec2 windowSize;
//
// out vec4 color;
//
// void main()
// {
//     float x = (position.x / windowSize.x) * 2.0 - 1.0;
//     float y = 1.0 - (position.y / windowSize.y) * 2.0;
//
//     gl_Position = vec4(x, y, 0.0, 1.0);
//
//     color = particleColor;
//
//     gl_PointSize = 1.0;
// }
#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 uv;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);

    uv = vec2(
        texCoord.x,
        1.0 - texCoord.y
    );
}
