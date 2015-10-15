// fragment shader for post deferred lighting
#version 330
#pragma optimize(off)
#pragma debug(on)
#define MAX_LIGHTS 10

out vec4 FBColor;
smooth in vec2 texCoord;

uniform mat4 mvM, projM;
uniform mat3 normM;

uniform int numLights;
	// [0] 0,1,2 light position, 3 if 0 is direction
	// [1] 0,1,2,3 colour intensities
	// [2] 0,1,2 attenuation coefficients, 3 is radius
	// [3] 0,1,2 cone direction, 3 is cone angle
uniform mat4 allLights[MAX_LIGHTS];

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normals;
uniform sampler2D eyepos;
uniform sampler2D reflects;

void main()
{
  FBColor = texture( depth, texCoord ) * texture( colour, texCoord )
      * texture( normals, texCoord ) * texture( eyepos, texCoord )
      * (projM * mvM * allLights[0][0]);
}
