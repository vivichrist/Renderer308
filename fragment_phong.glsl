// fragment shader for simple Phong Lighting model
#version 330
#define MAX_LIGHTS 10

out vec4 FBColor;

in VertexData {
	smooth vec2 vUV;
	smooth vec3 vNormal;
	smooth vec3 vView;
} fin;

uniform mat4 mvM;
uniform mat3 normM;
uniform sampler2D image;

uniform int numLights;
	// 0,1,2,3 light position
	// 0,1,2,3 specular intensities
	// 0,1,2 attenuation coefficients, 3 is ambient coefficient
	// 0,1,2 cone direction, 3 is cone angle
uniform mat4 allLights[MAX_LIGHTS];

uniform vec3 matAmb;
uniform vec4 matSpec; // specular intensities, shininess is w

void main()
{
  FBColor = vec4(0);
  float diff = 0; // accumulated diffuse intensity
  float spec = 0; // accumulated specular intensity
  vec3 specIntense = vec3(0);
  for ( int i = 0; i<numLights; ++i )
  {
    float d = 0; // current diffuse intensity
	float s = 0; // current specular intensity
    float att = 0; // attenuation
    vec3 lightDir = vec3(0); // direction to light from fragment
    mat4 light = allLights[i]; // chosen light

    if ( light[0].w == 1.0 ) // point light
    {
      vec4 lgt4 = mvM * light[0]; // light position in eye coordinates
      lightDir = (lgt4.xyz / lgt4.w) - fin.vView;
      float dist = length( lightDir );
      lightDir = normalize(lightDir);
      // check angle between light direction and spotlight direction.
      if ( light[3].w == 0.0 || degrees( acos( dot( normM * light[3].xyz, -lightDir ))) < light[3].w )
      { // spotlight
          // Dot product gives us diffuse intensity
          d = max(0.0, dot(normalize(fin.vNormal), lightDir));
          att = 1.0/( light[2].x + light[2].y*dist + light[2].z*dist*dist );
      }
    }
    else
    { // directional light
        lightDir = -normalize( normM * light[0].xyz );
        // Dot product gives us diffuse intensity
        d = max(0.0, dot(normalize(fin.vNormal), lightDir));
        att = 1.0;
    }
    if ( d > 0.0 )
    {
      // Halfway Normal
      vec3 halfway = normalize( lightDir - normalize(fin.vView) );
      // specular
      s = max( 0.0, dot( normalize(fin.vNormal), halfway ) );
      specIntense = max( specIntense, light[1].xyz * d * att );
      spec = max( spec, s );
    }
    diff = max( diff, att * d );
  }// end for each light

  vec3 specular = vec3(0);
  vec4 diffuse = texture( image, fin.vUV );
  // If the diffuse light is zero, don’t even bother with the pow function
  if ( diff > 0 )
  {
    specular = pow( spec, matSpec.w * 128.0 ) * (specIntense * diffuse.xyz );
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  FBColor = max( vec4( matAmb, 1 ), diff * diffuse )
  			+ vec4(spec * specular * matSpec.xyz, 1.0);
}
