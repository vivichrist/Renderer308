// fragment shader for simple Phongs Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

layout(location = 1) out vec4 colour;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 spec;

smooth in vec2 vUV;
smooth in vec3 vNormal;
smooth in vec3 vView;
smooth in vec3 vTangentLightPos;
smooth in vec3 vTangentFragPos;
smooth in vec3 vTangentView;
smooth in vec3 vTangentNormal;

//layout(depth_greater) out float gl_FragDepth;

uniform mat4 mvM;
uniform mat3 light;
uniform float parallaxScale;
uniform float parallaxMinLayer;
uniform float parallaxMaxLayer;
uniform sampler2D image;
uniform sampler2D normalmap;
uniform sampler2D heightmap;
uniform sampler2D depth;

//gl_FragDepth


// Gets the UV Coordinate of the given vector that is in tangent space. Against the given Texture coordinate
// Outputs the given parallaxHeight of the pixel to the parallaxHeight variable
vec2 parallaxMapping(in vec3 view, in vec2 texCoords, out float parallaxHeight){

   // determine number of layers from angle between V and N
   float numLayers = mix(parallaxMaxLayer, parallaxMinLayer, abs(dot(vec3(0.0, 0.0, 1.0), view)));

   // height of each layer
   float layerHeight = 1.0 / numLayers;

   // depth of current layer
   float currentLayerHeight = 0;

   // shift of texture coordinates for each iteration
   vec2 dtex = view.xy / -view.z / numLayers*parallaxScale;

   // current texture coordinates
   vec2 currentTextureCoords = texCoords;

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

// Gets the UV coodinate that should be displaced on the screen
vec2 getUVCoordinate(out float parallaxHeight){

	// Get UV of the current pixel
	vec2 uv = (vUV*4-50)+vec2(1.5,1); // Added offset to fix table texture on the screen

	if( parallaxScale != 0.0 ){
		// Get parallax' offset of this pixel
		vec3 viewDir = normalize(vTangentView - vTangentFragPos);
		uv = parallaxMapping(viewDir, uv, parallaxHeight);
	}

	return uv;

}

float parallaxSoftShadowMultiplier(in vec3 L, in vec2 initialTexCoord,
                                       in float initialHeight)
{
   float shadowMultiplier = 1;

   // calculate lighting only for surface oriented to the light source
   if(dot(vec3(0, 0, 1), L) > 0)
   {
      // calculate initial parameters
      float numSamplesUnderSurface	= 0;
      shadowMultiplier	= 0;
      float numLayers	= mix(parallaxMaxLayer, parallaxMinLayer, abs(dot(vec3(0, 0, 1), L)));
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

void main()
{
	// Multiply intensity by diffuSse color, force alpha to 1.0 and add in ambient light
	float parallaxHeight = 0;
	vec2 uv = getUVCoordinate(parallaxHeight);

	// Normal
	vec3 n = texture(normalmap, uv).rgb*2-1;
	n = normalize(n);

    // Dot product gives us diffuse intensity
	vec4 lightPos = mvM*vec4(light[0],1); // w=1 point light
    vec3 lightDir = normalize(lightPos.xyz - vView);
    vec3 tangentLightDir = normalize(vTangentLightPos - vTangentFragPos);
    float lightIntensity;

    colour = vec4(0);
    if( parallaxScale != 0.0 ){
    	lightIntensity = parallaxSoftShadowMultiplier(tangentLightDir.xyz,uv,parallaxHeight);
    	//colour = vec4(f,f,f,1);
    }
    else{
    	// Dot product gives us diffuse intensity
		vec4 lightPos = mvM*vec4(light[0],1); // w=1 point light
		vec3 lightDir = normalize(lightPos.xyz - vView);
		vec3 tangentLightDir = normalize(vTangentLightPos - vTangentFragPos);
		lightIntensity = max(0.0, dot(lightDir,vNormal));
    }

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    colour += vec4(light[1],1)*lightIntensity * texture( image, uv ) + vec4(0,0,0,0);

    // Specular Light
    vec3 halfway = normalize(lightDir - normalize(vView));

    float spec = max(0.0, dot(n,halfway));


    // If the diffuse light is zero, don’t even bother with the pow function
    if ( lightIntensity > 0 )
    {
        float fSpec = pow(spec, 128.0);
        colour.rgb += vec3(fSpec, fSpec, fSpec);
    }


    //
    // getUVCoordinate Has to be after all the outs are assigned!!
    //

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    //float parallaxHeight;
    //vUV = getUVCoordinate(parallaxHeight);
    //colour = (vec4(light[1]*lightIntensity,0))*texture( image, uv );//-parallaxHeight/2*(1-(parallaxScale));
}
