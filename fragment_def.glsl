// fragment shader for simple Phongs Lighting model
#version 330
#pragma optimize(off)
#pragma debug(on)

layout(location = 1) out vec4 colour;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 eye;

smooth in vec2 vUV;
smooth in vec3 vNormal;
smooth in vec3 vView;
smooth in vec3 vLightDir;
smooth in vec3 vTangentFragPos;
smooth in vec3 vTangentView;

uniform mat4 mvM;
uniform float parallaxScale;
uniform float parallaxMinLayer;
uniform float parallaxMaxLayer;
uniform sampler2D image;
uniform sampler2D normalmap;
uniform sampler2D heightmap;

// Converts v to tangent space
/*vec3 tangent3(in vec3 v, in vec3 normal){

	vec3 n = normalize(normal*vNormal);
	vec3 t = normalize(normal*vtangent);
	vec3 b = cross(t,n);
	mat3 mat = transpose(mat3(t,b,n));//t.x,b.x,n.x,t.y,b.y,n.y,t.z,b.z,n.z);

	return mat*v;
}*/

// Gets the UV Coordinate of the given vector that is in tangent space. Against the given Texture coordinate
// Outputs the given parallaxHeight of the pixel to the parallaxHeight variable
vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight){

   // determine number of layers from angle between V and N
   float numLayers = mix(parallaxMaxLayer, parallaxMinLayer, abs(dot(vec3(0.0, 0.0, 1.0), V)));

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

//vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
//{
//    float height =  texture(depthMap, texCoords).r;
//    return texCoords - viewDir.xy / viewDir.z * (height * height_scale);
//}

vec2 ParallaxMapping2(vec2 texCoords, vec3 viewDir)
{
	// number of depth layers
	const float height_scale = 1;
	// number of depth layers
	const float minLayers = 1;
	const float maxLayers = 1;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(heightmap, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(heightmap, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
}

// Gets the UV coodinate that should be displaced on the screen
vec2 getUVCoordinate(out float parallaxHeight){

	// Get UV of the current pixel
	vec2 uv = (vUV*4-50)+vec2(1.5,1); // Added offset to fix table texture on the screen

	// Get parallax' offset of this pixel
	vec3 viewDir = normalize(vTangentView - vTangentFragPos);
	//vec2 uvP = ParallaxMapping2(uv, viewDir);
	vec2 uvP = parallaxMapping(viewDir, uv, parallaxHeight);

	// Don't draw if it's out of bounds
	//if(uvP.x > 1.0 || uvP.y > 1.0 || uvP.x < 0.0 || uvP.y < 0.0)
	  //  discard;

	return uvP;

}

void main()
{
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vNormal), vLightDir)) * 4;

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    colour = max( diff, 0.05 ) * texture( image, vUV ) + vec4(0.3,0.3,0.3,1);

    // Specular Light
    vec3 halfway = normalize(vLightDir - normalize(vView));

    float spec = max(0.0, dot(normalize(vNormal), halfway));
    eye = vec4(0);
    // If the diffuse light is zero, don’t even bother with the pow function
    if ( diff > 0 )
    {
        float fSpec = pow(spec, 128.0);
        colour.rgb += vec3(fSpec, fSpec, fSpec);
        eye = vec4(fSpec, fSpec, fSpec, 1);
    }

    normal = vec4( vNormal, 1 );

    //
    // getUVCoordinate Has to be after all the outs are assigned!!
    //

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    float parallaxHeight;
    vec2 uv = getUVCoordinate(parallaxHeight);
    colour = texture( image, uv )-parallaxHeight/2*(1-(parallaxScale));

}
