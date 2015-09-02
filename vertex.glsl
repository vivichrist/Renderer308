// Vertex shader for simple Phong Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 NormalMatrix;
// Matrix to transform normals. This is the transpose of the
// inverse of the upper leftmost 3x3 of the modelview matrix.

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TextCoord;
layout (location = 2) in vec3 Normal;

smooth out vec3 PositionInterp;
smooth out vec3 NormalInterp;
smooth out vec3 ColorInterp;
// smooth out vec2 TextCoordInterp;

void main()
{
    PositionInterp = (Projection * (ModelView * vec4(Position, 1.0))).xyz;
    ColorInterp = vec3( 1,0,0 ); // Red
    // Normals transform
    NormalInterp = normalize(NormalMatrix * Normal);
    // TextCoordInterp = TextCoord;
}