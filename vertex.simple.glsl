#version 330 core

uniform mat4 mvM;
uniform mat4 projM;

layout (location = 0) in vec3 position;

void main(void)
{
    gl_Position = projM * mvM * vec4(position, 1);
}