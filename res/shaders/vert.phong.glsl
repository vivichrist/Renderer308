// Vertex shader for simple Phongs Lighting model
#version 330
layout (std140, binding = 0) uniform Cam
{
	uniform mat4 mvM;
	uniform mat4 projM;
	uniform mat3 normM; // Matrix to transform normals.
	uniform vec3 lightP;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out VertexData
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vView;
	vec3 vLightDir;
} v_out;

void main(void)
{
	// Get surface normal in eye coordinates
	v_out.vNormal = normalize(normM * normal);
	v_out.vUV = texCoord;
	// Get vertex position in eye coordinates
	vec4 pos4 = cam.mvM * vec4( position, 1 );
	v_out.vView = pos4.xyz / pos4.w;
	// Get light position in eye coordinates
	vec4 lgt4 = cam.mvM * vec4( lightP, 1 );
    vec3 lpos = lgt4.xyz / lgt4.w;
	
	v_out.vLightDir = normalize(lpos - v_out.vView);
	// transform the geometry!
	gl_Position = projM * pos4;
}