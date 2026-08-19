#version 330 core

uniform vec2 position;
uniform vec2 windowSize;

void main()
{
    vec2 normalized = position / windowSize;
    normalized = normalized * 2.0 - 1.0;
    normalized.y  = -normalized.y;

    gl_Position = vec4(normalized, 0.0, 1.0);
    gl_PointSize = 10.0;
}
