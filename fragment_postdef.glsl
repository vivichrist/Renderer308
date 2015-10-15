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
  FBColor = vec4(0);
  vec3 diff = vec3(0); // accumulated diffuse intensity
  float spec = 0; // accumulated specular intensity
  vec3 specIntense = vec3(0);
  for ( int i = 0; i<numLights; ++i )
  {
  	mat4 light = allLights[i]; // Chosen Light
  	vec3 fragPos = vec3( texCoord, texture( depth, texCoord ).x );
  	vec3 lightSS = (projM * mvM * ( vec4( light[0].xyz, 1 ))).xyz;
  	float distance = length( lightSS - fragPos );
    if( distance < light[2].w || light[0].w == 0.0 )
    {
	    float d = 0; // current diffuse intensity
	    float s = 0; // current specular intensity
	    float att = 0; // attenuation
	    vec3 lightDir = vec3(0); // direction to light from fragment
	    vec3 normal = texture( normals, texCoord ).xyz;

	    if ( light[0].w == 1.0 ) // Point Light
	    {// light direction in ss
	      lightDir = lightSS - fragPos;
	      float dist = length( lightDir );
	      lightDir = normalize(lightDir);
	      // check angle between light direction and spotlight direction.
	      if ( light[3].w == 0.0 ||
	      		degrees( acos( dot( normM * light[3].xyz, -lightDir ))) < light[3].w )
	      { // spotlight
	          // Dot product gives us diffuse intensity
	          att = 1.0 / ( light[2].x + light[2].y * dist + light[2].z * dist * dist );
	          d = max(0.0, dot(normalize(normal), lightDir));// * att;
	      }
	    }
	    else // Directional Light
	    {
	        lightDir = -normalize( normM * light[0].xyz );
	        // Dot product gives us diffuse intensity
	        d = max(0.0, dot(normalize(normal), lightDir));
	        att = 1.0;
	    }
	    vec3 lightstrength = min( att, d ) * light[1].xyz;
	    if ( d > 0.0 )
	    {
	      // Halfway Normal
	      vec3 halfway = normalize( lightDir - normalize( texture( eyepos, texCoord ).xyz ) );
	      // specular
	      s = max( 0.0, dot( normalize(normal), halfway ) );
	      specIntense = max( specIntense, lightstrength );
	      spec = max( spec, s );
	    }
	    diff = max( diff, lightstrength );
    }
  }// end for each light
  vec3 specular = vec3(0);
  vec4 texel = texture( colour, texCoord );
  vec4 diffuse = vec4( diff, 1 ) * texel;
  vec4 ambient = vec4(0.05, 0.05, 0.05, 1) * texel;

  // If the diffuse light is zero, don’t even bother with the pow function
  if ( diff.x > 0 || diff.y > 0 || diff.z > 0 )
  {
    specular = pow( spec, 512.0 ) * (specIntense * diffuse.xyz );
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  FBColor = max( ambient, diffuse ) + vec4(spec * specular, 1.0) + vec4(1,1,1,1);
}
