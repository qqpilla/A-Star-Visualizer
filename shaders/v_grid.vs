#version 330 core

layout (location = 0) in float end_pos;
layout (location = 1) in float offset;

void main()
{
    gl_Position = vec4(vec2(offset, end_pos), 0.0, 1.0);
}