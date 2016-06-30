// Vertex shader for simple Phong Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

uniform mat4 mvM;
uniform mat4 projM;
uniform mat3 normM; // Matrix to transform normals.
uniform mat3 light;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 instpos;
layout (location = 4) in vec3 instcolor;

smooth out vec2 vUV;
smooth out vec3 vNormal;
smooth out vec3 vView;
smooth out vec3 vPos;
smooth out vec3 vLightDir;

void main() {

    // Get vertex position in eye coordinates
    vec4 pos4 = mvM * vec4( position + instpos, 1 );
    
    vec4 lit4 = mvM * vec4( light[0], 1 );

    vUV = texCoord;
    vNormal = normM * normal;
    vPos = pos4.xyz;
    vView = -pos4.xyz;
    vLightDir = normM * normalize(-light[0]);

    gl_Position = projM * pos4;
}