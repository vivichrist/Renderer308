// Vertex shader for simple Phongs Lighting model
#version 330

uniform mat4 mvM, projM, viewM;
uniform mat3 normM; // Matrix to transform normals.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 instpos;
layout (location = 5) in vec3 tangent;
layout (location = 6) in vec3 bitangent;

in vec3 test;

out VertexData {
	smooth out vec4 vWorldPos;
    smooth out vec3 vPos;
	smooth out vec2 vUV;
	smooth out vec3 vNormal;
	smooth out vec3 vView;
	smooth out vec3 tangent;
	smooth out vec3 bitangent;
} vout;

void main(void)
{
	// Get surface normal in eye coordinates
	vout.vNormal = normalize(normM * normal);

	vout.vUV = texCoord;

	// Get vertex position in eye coordinates
	vout.vWorldPos = mvM * vec4( position + instpos, 1 );
	vout.vView = vout.vWorldPos.xyz / vout.vWorldPos.w;


	// reflection calculation
	mat3 invCam = transpose( mat3( mvM ) );
	vout.vPos = normalize( invCam * reflect( vout.vView, vout.vNormal ) );

	//vec3 tangent2 = vec3(1.0, 1.0, 1.0);
	//
	// Parallax
	//
	vec3 v1 = cross(normal,vec3(0.0,0.0,1.0));
	vec3 v2 = cross(normal,vec3(0.0,1.0,0.0));
	if( length(v1) > length(v2) )
		vout.tangent = v1;
	else
		vout.tangent = v2;
	//vout.tangent = tangent;


   // transform to world space
   /*vec4 worldPosition = pos4;
   vec3 worldNormal	  = normalize(mvM[0].xyz * normal);
   vec3 worldTangent  = normalize(mvM[0].xyz * tangent.xyz);

   // calculate vectors to the camera and to the light
   //vec3 worldDirectionToLight	= normalize(u_light_position - worldPosition.xyz);
   mat4 viewModel = inverse(mvM);
   vec3 cameraPos = viewModel[3].xyz; // Might have to divide by w if you can't assume w == 1
   vec3 worldDirectionToCamera	= normalize(cameraPos - worldPosition.xyz);

   // calculate bitangent from normal and tangent
   vec3 worldBitangnent	= cross(worldNormal, worldTangent) * 1;*/

   // transform direction to the light to tangent space
   /*o_toLightInTangentSpace = vec3(
         dot(worldDirectionToLight, worldTangent),
         dot(worldDirectionToLight, worldBitangnent),
         dot(worldDirectionToLight, worldNormal)
      );*/

   // transform direction to the camera to tangent space
   /*vout.tangent = vec3(
         dot(worldDirectionToCamera, worldTangent),
         dot(worldDirectionToCamera, worldBitangnent),
         dot(worldDirectionToCamera, worldNormal)
      );
*/

	// transform the geometry!
	gl_Position = projM * vout.vWorldPos;
}