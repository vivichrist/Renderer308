// Vertex shader for simple Phongs Lighting model
#version 330

uniform mat4 mvM;
uniform mat4 projM;
uniform mat3 normM; // Matrix to transform normals.
uniform vec3 lightP;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

smooth out vec3 vLightDir;
smooth out vec2 vUV;
smooth out vec3 vNormal; //interpolate the Normal

void main(void)
{
	// Get surface normal in eye coordinates
	vNormal = normalize(normM * normal);
	vUV = texCoord;

	// Get vertex position in eye coordinates
	vec4 pos4 = mvM * vec4( position, 1 );
	vec3 eye3 = pos4.xyz / pos4.w;

	// Get vector to light source
	vLightDir = normalize(lightP - eye3);

	// Don’t forget to transform the geometry!
	gl_Position = projM * pos4;
}