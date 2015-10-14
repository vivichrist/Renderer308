// Basic vertex shader for parallax mapping
#version 330

// attributes
layout(location = 0) in vec3 position;	// xyz - position
layout(location = 1) in vec3 normal;	// xyz - normal
layout(location = 2) in vec2 texcoord0;	// xy - texture coords
layout(location = 3) in vec3 instpos;
layout(location = 4) in vec4 tangent; // xyz - tangent, w - handedness

// uniforms
uniform mat4 mvM;
uniform mat4 u_view_mat;
uniform mat4 projM;
uniform mat3 normM;
uniform vec3 u_light_position;
uniform vec3 u_camera_position;

// data for fragment shader
out vec2 o_texcoords;
out vec3 o_toLightInTangentSpace;
out vec3 o_toCameraInTangentSpace;

///////////////////////////////////////////////////////////////////

void main(void)
{
   // transform to world space
   vec4 worldPosition = mvM * vec4(position+instpos, 1);
   vec3 worldNormal	  = normalize(normM * normal);
   vec3 worldTangent  = normalize(normM * tangent.xyz);

   // calculate vectors to the camera and to the light
   vec3 worldDirectionToLight	= normalize(u_light_position - worldPosition.xyz);
   vec3 worldDirectionToCamera	= normalize(u_camera_position - worldPosition.xyz);

   // calculate bitangent from normal and tangent
   vec3 worldBitangnent	= cross(worldNormal, worldTangent) * tangent.w;

   // transform direction to the light to tangent space
   o_toLightInTangentSpace = vec3(
         dot(worldDirectionToLight, worldTangent),
         dot(worldDirectionToLight, worldBitangnent),
         dot(worldDirectionToLight, worldNormal)
      );

   // transform direction to the camera to tangent space
   o_toCameraInTangentSpace= vec3(
         dot(worldDirectionToCamera, worldTangent),
         dot(worldDirectionToCamera, worldBitangnent),
         dot(worldDirectionToCamera, worldNormal)
      );

   // pass texture coordinates to fragment shader
   o_texcoords	= texcoord0;

   // calculate screen space position of the vertex
   gl_Position	= projM * u_view_mat * worldPosition;
}