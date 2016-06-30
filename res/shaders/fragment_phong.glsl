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
uniform vec4 material; // intensities, shininess is w
uniform sampler2D image;

uniform int numLights;
	// 0,1,2,3 light position
	// 0,1,2,3 specular intensities
	// 0,1,2 attenuation coefficients, ambient coefficient is w;
	// 0,1,2 cone direction, 3 cone angle
uniform mat4 allLights[MAX_LIGHTS];


void main()
{
  FBColor = 0;
  float diff = 0;
  float spec = 0;
  vec3 specIntense = 0;
  for ( int i = 0; i<numLights; ++i )
  {
    vec3 lightDir = vec3();
    mat4 light = allLights[i];
    if ( light[0].w == 1.0 )
    { // point light
      vec4 lgt4 = mvM * light[0];
      lightDir = normalize((lgt4.xyz / lgt4.w) - vView);

      // Dot product gives us diffuse intensity
      diff = max(0.0, dot(normalize(vNormal), lightDir));

      if ( light[3].w != 0.0 )
      { // spotlight
        // TODO: ... check angle between light direction and spotlight direction.
        // attenuate from some angle
      }
    else
    { // directional light
        lightDir = normalize( -light[0] )
        // Dot product gives us diffuse intensity
        diff = max(0.0, dot(normalize(vNormal), lightDir));
    }
    if ( diff > 0 )
    {
      // Halfway Normal
      vec3 halfway = normalize( lightDir - normalize(vView) );
      // specular
      spec += max( 0.0, dot( normalize(vNormal), halfway ) );
      specIntense = max( specIntense, light[1].xyz );
    }
    // If the diffuse light is zero, don’t even bother with the pow function
    }
  }

  if ( diff > 0 )
  {
    specular = pow( spec, material.w ) * specIntense;
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  vec3 diffuse = texture( image, vUV );
  FBColor = max( material.w, diff ) * diffuse + spec * specular;
}