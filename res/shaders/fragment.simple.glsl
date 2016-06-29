#version 330 core

uniform sampler2D image;

out vec4 color;

void main(void)
{
    color = texture( image, vec2(0) );
}