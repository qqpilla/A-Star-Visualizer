#version 330 core

layout (location = 0) in vec2 coords;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 offset;

out vec3 v_color;

void main()
{
    gl_Position = vec4(coords + offset, 0.0, 1.0);
    v_color = color;
}