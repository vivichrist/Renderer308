// fragment shader for simple Phong Lighting model
#version 330 core
#extension GL_EXT_geometry_shader4 : enable

#define MAX_LIGHTS 10

layout(location = 0) out vec4 FB0;

in VertexData {
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
    flat mat4 mvM;
    flat mat3 normM;
    flat int side;
} fin;

uniform vec4 matAmb; // ambient intensities, reflection is w
uniform vec4 matSpec; // specular intensities, shininess is w

uniform int numLights;
uniform mat4 allLights[MAX_LIGHTS];
    // [0] 0,1,2 light position, 3 if 0 is direction
    // [1] 0,1,2,3 colour intensities
    // [2] 0,1,2 attenuation coefficients
    // [3] 0,1,2 cone direction, 3 is cone angle

uniform sampler2D image;

void main()
{
  FBColor = vec4(0);
  vec3 diff = vec3(0); // accumulated diffuse intensity
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
      vec4 lgt4 = fin.mvM * light[0]; // light position in eye coordinates
      lightDir = (lgt4.xyz / lgt4.w) - fin.vView;
      float dist = length( lightDir );
      lightDir = normalize(lightDir);
      // check angle between light direction and spotlight direction.
      if ( light[3].w == 0.0 ||
            degrees( acos( dot( fin.normM * light[3].xyz, -lightDir ))) < light[3].w )
      { // spotlight
          att = 1.0/( light[2].x + light[2].y*dist + light[2].z*dist*dist );
          // Dot product gives us diffuse intensity
          d = max(0.0, dot(normalize(fin.vNormal), lightDir));// * att;
      }
    }
    else
    { // directional light
        lightDir = -normalize( fin.normM * light[0].xyz );
        // Dot product gives us diffuse intensity
        d = max(0.0, dot(normalize(fin.vNormal), lightDir));
        att = 1.0;
    }
    vec3 lightstrength = min( att, d ) * light[1].xyz;
    if ( d > 0.0 )
    {
      // Halfway Normal
      vec3 halfway = normalize( lightDir - normalize(fin.vView) );
      // specular
      s = max( 0.0, dot( normalize(fin.vNormal), halfway ) );
      specIntense = max( specIntense, lightstrength );
      spec = max( spec, s );
    }
    diff = max( diff, lightstrength );
  }// end for each light

  vec3 specular = vec3(0);
  vec4 diffuse = texture( image, fin.vUV * 5.0 );
  // If the diffuse light is zero, don’t even bother with the pow function
  if ( diff.x > 0 && diff.y > 0 && diff.z > 0 )
  {
    specular = pow( spec, matSpec.w * 512.0 ) * (specIntense * diffuse.xyz );
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  FB0 = max( 0.25 * vec4( matAmb.xyz, 1 ), vec4( diff, 1 ) * diffuse )
                         + vec4(spec * specular * matSpec.xyz, 1.0);
}