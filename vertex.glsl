// Vertex shader for simple Phong Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

uniform mat4 mvM;
uniform mat4 projM;
uniform mat3 normM; // Matrix to transform normals.
uniform mat3 light;
uniform vec3 viewP;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 instpos;
layout (location = 4) in vec3 instcolor;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

smooth out vec2 vUV;
smooth out vec3 vNormal;
smooth out vec3 vView;
smooth out vec3 vTangentLightPos;
smooth out vec3 vTangentFragPos;
smooth out vec3 vTangentView;
smooth out vec3 vTangentNormal;

void main(void)
{
    // Get surface normal in eye coordinates
	vNormal = normalize(normM * normal);

    vUV = texCoord;

    // Get vertex position in eye coordinates
    vec4 pos4 = mvM * vec4( position + instpos, 1 );
    vView = pos4.xyz / pos4.w;

    // Tangent Space
    vec3 T = normalize(mat3(mvM) * tangent);
	vec3 B = normalize(mat3(mvM) * bitangent);
	vec3 N = normalize(mat3(mvM) * normal);
	mat3 TBN = mat3(T, B, N);

	vTangentView     = TBN * mat3(mvM)*viewP;
	vTangentLightPos = TBN * mat3(mvM)*light[0];
	vTangentFragPos  = TBN * pos4.xyz;
	vTangentNormal   = TBN * vNormal;

    // transform the geometry!
    gl_Position = projM * pos4;
}