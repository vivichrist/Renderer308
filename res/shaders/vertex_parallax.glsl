// Vertex shader for Parallax
#version 330
#pragma optimize(off)
#pragma debug(on)

uniform mat4 mvM, projM;
uniform mat3 normM; // Matrix to transform normals.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 binormalWS;
layout (location = 4) in vec3 tangentWS;

uniform float baseTextureRepeat;
uniform float heightMapRange;
uniform vec3 lightPos;

out VertexData {
    smooth out vec3 vPos;
	smooth out vec2 vUV;
	smooth out vec3 vNormal;
	smooth out vec3 vView;
	smooth out vec3 vLightTang; // Light vector in tangent space, not normalized
	smooth out vec3 vViewTang; // View vector in tangent space, not normalized
	smooth out vec2 vParallaxTang; // Parallax offset vector in tangent space
} vout;

void main(void)
{
	// Propagate the world vertex normal through
	vout.vNormal = normalize(normM * normal);

	// Propagate texture coordinate through
	vout.vUV = texCoord;

	// Get vertex position in eye coordinates
	vec4 pos4 = mvM * vec4( position, 1 );
	vout.vView = pos4.xyz / pos4.w;

	// reflection calculation
	mat3 invCam = transpose( mat3( mvM ) );
	vout.vPos = normalize( invCam * reflect( vout.vView, vout.vNormal ) );

	// Uncomment this to repeat the texture
	// vout.vUV *= fBaseTextureRepeat;
	// Compute denormalised light vector in world space:
	vec3 lightN = lightPos - position;

	// Normalize the light and view vectors and transform it to the tangent space
	mat3 mWorldToTangent = mat3( i.vTangentWS, i.vBinormalWS, i.vNormalWS );

	// Propagate the view and the light vectors in tangent space
	vout.vLightTang = mWorldToTangent * lightN;
	vout.vViewTang = mWorldToTangent * vView;

	// Compute the ray direction for intersecting the height field
	// profile with current view ray. See the above paper for derivation
	// of this computation.
	// Compute initial parallax displacement direction:
	vec2 parallaxDir = normalize( vout.vView.xy );

	// The length of this vector determines the displacement limit
	float len = length( vout.vViewTang );
	float parallaxLen = sqrt( len * len - vout.vViewTang.z * vout.vViewTang.z )
							/ Out.vViewTang.z;

	// Compute the actual reverse parallax displacement vector:
	vout.vParallaxTang = parallaxDir * parallaxLen;

	// Need to scale the amount of displacement to account for
	// different height ranges in height maps. This is controlled by
	// an artist-editable parameter:
	vout.vParallaxTang *= heightMapRange;

	// transform the geometry!
	gl_Position = projM * pos4;
}