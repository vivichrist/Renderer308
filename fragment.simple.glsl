#version 330 core

in vec3 vcolor;

out vec4 color;

void main(void)
{
    color.rgb = vcolor;
    color.a = 1;
}