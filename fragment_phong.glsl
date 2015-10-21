// fragment shader for simple Phong Lighting model
#version 330
#define MAX_LIGHTS 10

out vec4 FBColor;

in VertexData {
    smooth vec3 vPos;
	smooth vec2 vUV;
	smooth vec3 vNormal;
	smooth vec3 vView;
} fin;

uniform float matCubemap; // cubemap
uniform float matNormal; // normalmap
uniform vec4 matAmb; // ambient intensities, reflection is w
uniform vec4 matSpec; // specular intensities, shininess is w

uniform mat4 mvM;
uniform mat3 normM;

uniform int numLights;
	// [0] 0,1,2 light position, 3 if 0 is direction
	// [1] 0,1,2,3 colour intensities
	// [2] 0,1,2 attenuation coefficients
	// [3] 0,1,2 cone direction, 3 is cone angle
uniform mat4 allLights[MAX_LIGHTS];

uniform sampler2D image;
uniform sampler2D normalmap;
uniform samplerCube eMap;
uniform sampler2D DepthTexture;

void main()
{
  // For normal map
  vec3 texcolor = vec3(texture2D(normalmap,fin.vUV));
  vec3 normal = fin.vNormal + (texcolor*matNormal);

  FBColor = vec4(0);
  vec3 diff = vec3(0); // accumulated diffuse intensity
  float spec = 0; // accumulated specular intensity
  vec3 specIntense = vec3(0);

  vec3 diff = vec3(0); // accumulated diffuse intensity
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
          att = 1.0/( light[2].x + light[2].y*dist + light[2].z*dist*dist );
          d = max(0.0, dot(normalize(normal), lightDir));// * att;
      }
    }
    else
    { // directional light
        lightDir = -normalize( normM * light[0].xyz );
        // Dot product gives us diffuse intensity
        d = max(0.0, dot(normalize(normal), lightDir));
        att = 1.0;
    }
    vec3 lightstrength = min( att, d ) * light[1].xyz;
    if ( d > 0.0 )
    {
      // Halfway Normal
      vec3 halfway = normalize( lightDir - normalize(fin.vView) );
      // specular
      s = max( 0.0, dot( normalize(normal), halfway ) );
      specIntense = max( specIntense, lightstrength );
      spec = max( spec, s );
    }
    diff = max( diff, lightstrength );
  }// end for each light

  vec3 specular = vec3(0);
  vec4 cm = texture( eMap, fin.vPos );
  vec4 dm = texture( image, fin.vUV * 5.0 );
  vec4 diffuse = vec4( (1.0 - matCubemap) * dm.xyz, 1 ) + vec4( matCubemap * cm.xyz, 1 );

  // If the diffuse light is zero, don’t even bother with the pow function
  if ( diff.x > 0 || diff.y > 0 || diff.z > 0 )
  {
    specular = pow( spec, matSpec.w * 512.0 ) * (specIntense * diffuse.xyz );
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  FBColor = max( 0.25 * vec4( matAmb.xyz, 1 ), vec4( diff, 1 ) * diffuse )
  			+ vec4(spec * specular * matSpec.xyz, 1.0);
}
