// basic fragment shader for Parallax Mapping
#version 330

// data from vertex shader
in vec2	texCoord;
in vec3	o_toLightInTangentSpace;
in vec3	o_toCameraInTangentSpace;

// textures
uniform sampler2D image;
uniform sampler2D normalmap;
uniform sampler2D heightmap;

// color output to the framebuffer
out vec4	resultingColor;

////////////////////////////////////////

// scale for size of Parallax Mapping effect
uniform float	parallaxScale; // ~0.1

//////////////////////////////////////////////////////
// Implements Parallax Mapping technique
// Returns modified texture coordinates, and last used depth
vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight)
{
   // get depth for this fragment
   float initialHeight = texture(heightmap, texCoord).r;

   // calculate amount of offset for Parallax Mapping
   vec2 texCoordOffset = parallaxScale * V.xy / V.z * initialHeight;

   // calculate amount of offset for Parallax Mapping With Offset Limiting
   texCoordOffset = parallaxScale * V.xy * initialHeight;

   // retunr modified texture coordinates
   return texCoord - texCoordOffset;
}

//////////////////////////////////////////////////////
// Implements self-shadowing technique - hard or soft shadows
// Returns shadow factor
float parallaxSoftShadowMultiplier(in vec3 L, in vec2 initialTexCoord,
                                       in float initialHeight)
{
   float shadowMultiplier = 1;

   const float minLayers = 15;
   const float maxLayers = 30;

   // calculate lighting only for surface oriented to the light source
   if(dot(vec3(0, 0, 1), L) > 0)
   {
      // calculate initial parameters
      float numSamplesUnderSurface	= 0;
      shadowMultiplier	= 0;
      float numLayers	= mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), L)));
      float layerHeight	= initialHeight / numLayers;
      vec2 texStep	= parallaxScale * L.xy / L.z / numLayers;

      // current parameters
      float currentLayerHeight	= initialHeight - layerHeight;
      vec2 currentTextureCoords	= initialTexCoord + texStep;
      float heightFromTexture	= texture(heightmap, currentTextureCoords).r;
      int stepIndex	= 1;

      // while point is below depth 0.0 )
      while(currentLayerHeight > 0)
      {
         // if point is under the surface
         if(heightFromTexture < currentLayerHeight)
         {
            // calculate partial shadowing factor
            numSamplesUnderSurface	+= 1;
            float newShadowMultiplier	= (currentLayerHeight - heightFromTexture) *
                                             (1.0 - stepIndex / numLayers);
            shadowMultiplier	= max(shadowMultiplier, newShadowMultiplier);
         }

         // offset to the next layer
         stepIndex	+= 1;
         currentLayerHeight	-= layerHeight;
         currentTextureCoords	+= texStep;
         heightFromTexture	= texture(heightmap, currentTextureCoords).r;
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

/////////
// STEEP PARALLAX
//
/*vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight)
{
   // determine number of layers from angle between V and N
   const float minLayers = 5;
   const float maxLayers = 15;
   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // depth of current layer
   float currentLayerHeight = 0;
   // shift of texture coordinates for each iteration
   vec2 dtex = parallaxScale * V.xy / V.z / numLayers;

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
}*/

//////////////////////////////////////////////////////
// Calculates lighting by Blinn-Phong model and Normal Mapping
// Returns color of the fragment
vec4 normalMappingLighting(in vec2 T, in vec3 L, in vec3 V, float shadowMultiplier)
{
   // restore normal from normal map
   vec3 N = normalize(texture(normalmap, T).xyz * 2 - 1);
   vec3 D = texture(image, T).rgb;

   // ambient lighting
   float iamb = 0.2;
   // diffuse lighting
   float idiff = clamp(dot(N, L), 0, 1);
   // specular lighting
   float ispec = 0;
   if(dot(N, L) > 0.2)
   {
      vec3 R = reflect(-L, N);
      ispec = pow(dot(R, V), 32) / 1.5;
   }

   vec4 resColor;
   resColor.rgb = D * (vec3(0.5,0.5,0.5) + (idiff + ispec) * pow(shadowMultiplier, 4));
   resColor.a = 1;

   return resColor;
}

/////////////////////////////////////////////
// Entry point for Parallax Mapping shader
void main()
{
   // normalize vectors after vertex shader
   vec3 V = normalize(o_toCameraInTangentSpace);
   vec3 L = normalize(o_toLightInTangentSpace);

   // get new texture coordinates from Parallax Mapping
   float parallaxHeight;
   vec2 T = parallaxMapping(V, texCoord, parallaxHeight);

   // get self-shadowing factor for elements of parallax
   float shadowMultiplier = parallaxSoftShadowMultiplier(L, T, parallaxHeight - 0.05);

   // calculate lighting
   resultingColor = vec4(1.0,0.0,0.0,1.0);//normalMappingLighting(T, L, V, shadowMultiplier);
}