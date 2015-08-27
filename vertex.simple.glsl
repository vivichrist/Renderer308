#version 330 core

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec3 vcolor;

void main(void)
{
    gl_Position = Projection * ModelView * vec4(Position, 1);
    vcolor = vec3( 1, 1, 0 );
}