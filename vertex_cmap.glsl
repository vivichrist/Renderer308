// Vertex shader for simple Phong shading with multiple Lights model
#version 330

uniform mat4 mvM[6], projM;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 instpos;
layout (location = 4) in vec3 instcolor;

out VertexData {
	smooth out vec2 vUV;
	smooth out vec3 vNormal;
	smooth out vec3 vView;
} vout;

void main(void)
{
	vout.vNormal = normal;
	vout.vUV = texCoord;
	// transform the geometry!
	gl_Position = vec4( position, 1 );
}