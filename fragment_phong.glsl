// fragment shader for simple Phong Lighting model
#version 330
#define MAX_LIGHTS 10

out vec4 FBColor;

in VertexData {
  smooth vec4 vWorldPos;
  smooth vec3 vPos;
	smooth vec2 vUV;
	smooth vec3 vNormal;
	smooth vec3 vView;
  smooth vec3 tangent;
  smooth vec3 bitangent;
} fin;

uniform float matNormal; // normal Intensity
uniform float matCubemap; // cubemap Intensity
uniform vec4 matAmb; // ambient intensities, reflection is w
uniform vec4 matSpec; // specular intensities, shininess is w

uniform mat4 mvM, projM;
uniform mat3 normM;

uniform int numLights;
	// [0] 0,1,2 light position, 3 if 0 is direction
	// [1] 0,1,2,3 colour intensities
	// [2] 0,1,2 attenuation coefficients
	// [3] 0,1,2 cone direction, 3 is cone angle
uniform mat4 allLights[MAX_LIGHTS];

uniform sampler2D image;
uniform sampler2D normalmap;
uniform sampler2D heightmap;
uniform samplerCube eMap;



vec3 tangent3(vec3 v, vec3 normal){

  //fin.tangent = normalize(vec3(0.1,0.1,0.0));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	vec3 n = normalize(normM*fin.vNormal);
	vec3 t = normalize(normM*fin.tangent);
	vec3 b = cross(t,n);
	mat3 mat = mat3(t,b,n);//t.x,b.x,n.x,t.y,b.y,n.y,t.z,b.z,n.z);

	return mat*v;
}

vec2 tangent2(vec2 v, vec3 normal){
	return tangent3(vec3(v.xy,0),normal).xy;
}

vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight)
{
   const float parallaxScale = 0.1;
   // determine number of layers from angle between V and N
   const float minLayers = 50;
   const float maxLayers = 50;
   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // depth of current layer
   float currentLayerHeight = 0;
   // shift of texture coordinates for each iteration
   vec2 dtex = V.xy / -V.z / numLayers*parallaxScale;

   // current texture coordinates
   vec2 currentTextureCoords = T;

   // get first depth from heightmap
   float heightFromTexture = texture(heightmap, currentTextureCoords).r;

   // while point is above surface
   while(heightFromTexture > currentLayerHeight)
   {
      // to the next layer
      currentLayerHeight += layerHeight;
      // shift texture coordinates along vector V
      currentTextureCoords -= dtex;
      // get new depth from heightmap
      heightFromTexture = texture(heightmap, currentTextureCoords).r;
   }

   // return results
   parallaxHeight = currentLayerHeight;
   return currentTextureCoords;
}

/*vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight){

  // Get height of pixel
  float height = texture2D(heightmap, T).r;

  float v = height * 1;
  vec3 eye = normalize(fin.vView);
  return T + (eye.xy + v);
 }*/

// vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight)
// {
//    const float parallaxScale = 1;

//    // get depth for this fragment
//    float initialHeight = texture(heightmap, T).r;

//    // calculate amount of offset for Parallax Mapping
//    vec2 texCoordOffset = parallaxScale * V.xy / V.z * initialHeight;

//    // calculate amount of offset for Parallax Mapping With Offset Limiting
//    //texCoordOffset = parallaxScale * V.xy * initialHeight;

//    // retunr modified texture coordinates
//    return T - texCoordOffset;
// }

float parallaxSoftShadowMultiplier(in vec3 L, in vec2 initialTexCoord,
                                       in float initialHeight)
{
   float shadowMultiplier = 1;
   const float parallaxScale = 1.0;

   const float minLayers = 15;
   const float maxLayers = 30;

   // calculate lighting only for surface oriented to the light source
   if(dot(vec3(0, 0, 1), L) > 0)
   {
      // calculate initial parameters
      float numSamplesUnderSurface  = 0;
      shadowMultiplier  = 0;
      float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), L)));
      float layerHeight = initialHeight / numLayers;
      vec2 texStep  = parallaxScale * L.xy / L.z / numLayers;

      // current parameters
      float currentLayerHeight  = initialHeight - layerHeight;
      vec2 currentTextureCoords = initialTexCoord + texStep;
      float heightFromTexture = texture(heightmap, currentTextureCoords).r;
      int stepIndex = 1;

      // while point is below depth 0.0 )
      while(currentLayerHeight > 0)
      {
         // if point is under the surface
         if(heightFromTexture < currentLayerHeight)
         {
            // calculate partial shadowing factor
            numSamplesUnderSurface  += 1;
            float newShadowMultiplier = (currentLayerHeight - heightFromTexture) *
                                             (1.0 - stepIndex / numLayers);
            shadowMultiplier  = max(shadowMultiplier, newShadowMultiplier);
         }

         // offset to the next layer
         stepIndex  += 1;
         currentLayerHeight -= layerHeight;
         currentTextureCoords += texStep;
         heightFromTexture  = texture(heightmap, currentTextureCoords).r;
      }

      // Shadowing factor should be 1 if there were no points under the surface
      if(numSamplesUnderSurface < 1)
      {
         shadowMultiplier = 1;
      }
      else
      {
         shadowMultiplier = 1.0 - shadowMultiplier;
      }
   }
   return shadowMultiplier;
}


void main()
{


  float parallaxHeight;
  //vec3 V = normalize(fin.vView);
  vec3 V = normalize(tangent3(fin.vWorldPos.xyz, fin.vNormal));
  //vec3 V = normalize(tangent3(fin.vView,fin.vNormal));
  vec2 location = parallaxMapping(V, fin.vUV*5.0, parallaxHeight);

  // For normal map
  vec3 normalColor = texture2D(normalmap,fin.vUV*5.0).xyz;
  //fin.vNormal += (normalColor*matNormal);

  float shadowMultiplier = 0;

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
      vec4 lgt4 = mvM * light[0]; // light position in eye coordinates
      lightDir = normalize(((mvM * light[0]).xyz / lgt4.w) - fin.vView);
      float dist = length( lightDir );
      lightDir = normalize(lightDir);
      // check angle between light direction and spotlight direction.
      if ( light[3].w == 0.0 || degrees( acos( dot( normM * light[3].xyz, -lightDir ))) < light[3].w )
      { // spotlight
          // Dot product gives us diffuse intensity
          att = 1.0/( light[2].x + light[2].y*dist + light[2].z*dist*dist );

          
          d = max(0.0, dot(normalize(fin.vNormal), lightDir));// * att;

          // parallax
          //d += parallaxSoftShadowMultiplier(lgt4.xyz, location, parallaxHeight - 0.05);
      }

    }
    else
    { // directional light
        lightDir = -normalize( normM * light[0].xyz );
        // Dot product gives us diffuse intensity
        d = max(0.0, dot(normalize(fin.vNormal), lightDir));

        // parallax
        //d += parallaxSoftShadowMultiplier(lightDir, location, parallaxHeight - 0.05);

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
  vec4 cm = texture( eMap, fin.vPos );
  //vec4 dm = texture( image, parallaxMapping(fin.vUV * 5.0) );
  vec4 dm = texture( image, location );

  vec4 diffuse = vec4( (1.0 - matCubemap) * dm.xyz, 1 ) + vec4( matCubemap * cm.xyz, 1 );

  // If the diffuse light is zero, don’t even bother with the pow function
  if ( diff.x > 0 && diff.y > 0 && diff.z > 0 )
  {
    specular = pow( spec, matSpec.w * 512.0 ) * (specIntense * diffuse.xyz );
  }
  // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
  FBColor = max( 0.25 * vec4( matAmb.xyz, 1 ), vec4( diff, 1 ) * diffuse )
  			+ vec4(spec * specular * matSpec.xyz, 1.0);
}
