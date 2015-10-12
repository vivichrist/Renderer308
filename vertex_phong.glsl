// Vertex shader for simple Phongs Lighting model
#version 330

uniform mat4 mvM, projM;
uniform mat3 normM; // Matrix to transform normals.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 instpos;
layout (location = 4) in vec3 instcolor;

out VertexData {
    smooth out vec3 vPos;
	smooth out vec2 vUV;
	smooth out vec3 vNormal;
	smooth out vec3 vView;
} vout;

void main(void)
{
	// Get surface normal in eye coordinates
	vout.vNormal = normalize(normM * normal);

	vout.vUV = texCoord;

	// Get vertex position in eye coordinates
	vec4 pos4 = mvM * vec4( position + instpos, 1 );
	vout.vView = pos4.xyz / pos4.w;

	// reflection calculation
	mat3 invCam = transpose( mat3( mvM ) );
	vout.vPos = normalize( invCam * reflect( vout.vView, vout.vNormal ) );

	// Normal map
	/*vec3 tangent;
	vec3 v1 = cross(normal,vec3(0.0,0.0,-1.0));
	vec3 v2 = cross(normal,vec3(0.0,-1.0,0.0));
	if( length(v1) > length(v2) )
		tangent = v1;
	else
		tangent = v2;
	vec3 n = normalize(normM*normal);
	vec3 t = normalize(normM*tangent);
	vec3 b = cross(n,t);
	mat3 mat = mat3(t.x,b.x,n.x,t.y,b.y,n.y,t.z,b.z,n.z);

	vec3 vector = normalize(lightPos-position);
	tangentSurface2light = mat*vector;

	vector = normalize(-position);
	tangentSurface2view = mat*vector;*/


	// transform the geometry!
	gl_Position = projM * pos4;
}