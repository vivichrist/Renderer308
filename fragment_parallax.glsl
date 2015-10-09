// fragment shader for deferred rendering
#version 330
#pragma optimize(off)
#pragma debug(on)

// if we need to write to the depth buffer, depth_greater means
// 'You will only make the depth larger, compared to gl_FragDepth.z​'
//layout (depth_greater) out float gl_FragDepth;

out vec4 colour;

in VertexData {
    smooth in vec3 vPos;
	smooth in vec2 vUV;
	smooth in vec3 vNormal;
	smooth in vec3 vView;
	smooth in vec3 vLightTang; // Light vector in tangent space, not normalized
	smooth in vec3 vViewTang; // View vector in tangent space, not normalized
	smooth in vec2 vParallaxTang; // Parallax offset vector in tangent space
} fin;

uniform mat4 mvM;
uniform mat3 normM;

// This parameter contains the dimensions of the height map / normal map
// pair and is used for determination of current mip level value:

uniform vec2 textureDims;

uniform int nLODThreshold;

uniform float shadowSoftening;
uniform float specularExponent;
uniform float diffuseBrightness;
uniform float heightMapRange;

uniform vec4 ambientColour;
uniform vec4 diffuseColour;
uniform vec4 specularColour;

uniform int nMinSamples;
uniform int nMaxSamples;

uniform sampler2D baseMap;
uniform sampler2D normalMap;

//......................................................................
// Function: ComputeIllumination
//
// Description: Computes phong illumination for the given pixel using
// its attribute textures and a light vector.
//......................................................................
void ComputeIllumination( in vec2 texCoord, in vec3 lightTS
						, in vec3 viewTS, in float occlusionShadow
						, out vec4 cFinalColor )
{
	// Sample the normal from the normal map for the given texture sample:
	vec3 normalTS = normalize( texture( normalMap, texCoord ) * 2 - 1 );

	// Sample base map:
	vec4 cBaseColor = texture( tBaseMap, texCoord );

	// Compute diffuse color component:
	vec4 diffuse = saturate( dot( normalTS, lightTS )) * diffuseColor;

	// Compute specular component:
	vec3 reflectTS = normalize( 2 * dot( viewTS, normalTS ) * normalTS - viewTS );
	float rdotL = dot( reflectTS, lightTS );
	vec4 specular = saturate( pow( rdotL, specularExponent )) * specularColour;
	cFinalColor = (( ambientColour + diffuse ) * baseColour + specular )
				   * occlusionShadow;
}

void main()
{
	// Normalize the interpolated vectors:
	vec3 vViewTang = normalize( fin.vViewTang );
	vec3 vView = normalize( fin.vView );
	vec3 vLightTang = normalize( fin.vLightTang );
	vec3 vNormal = normalize( fin.vNormal );
	colour = vec4( 0, 0, 0, 1 );

	// Adaptive in-shader level-of-detail system implementation.
	// Compute the current mip level explicitly in the pixel shader
	// and use this information to transition between different levels
	// of detail from the full effect to simple bump mapping.
	// Compute the current gradients
	vec2 texCoordsPerSize = fin.vUV * textureDims;

	// Compute all 4 derivatives in x and y in a single instruction to optimize
	vec2 dxSize, dySize;
	vec2 dx, dy;
	vec4( dxSize, dx ) = dFdx( vec4( texCoordsPerSize, fin.vUV ) );
	vec4( dySize, dy ) = dFdy( vec4( texCoordsPerSize, fin.vUV ) );

	// mip level integer portion, mip level fractional amount for
	// blending in between levels
	float mipLevel;
	float mipLevelInt;
	float mipLevelFrac;

	float minTexCoordDelta;
	vec2 texCoords;

	// Find min of change in u and v across quad: compute du and dv
	// magnitude across quad
	texCoords = dxSize * dxSize + dySize * dySize;

	// Standard mipmapping uses max here
	minTexCoordDelta = max( texCoords.x, texCoords.y );

	// Compute the current mip level (* 0.5 is effectively
	// computing a square root before )
	mipLevel = max( 0.5 * log2( minTexCoordDelta ), 0 );

	// Start the current sample located at the input texture
	// coordinate, which would correspond to computing a bump
	// mapping result:
	vec2 texSample = fin.vUV;

	// Multiplier for visualizing the level of detail
	vec4 cLODColoring = vec4( 1, 1, 3, 1 );
	float occlusionShadow = 1.0;

	if ( mipLevel <= (float) nLODThreshold )
	{
		//===============================================//
		// Parallax occlusion mapping offset computation //
		//===============================================//
		// Utilize dynamic flow control to change the number of samples
		// per ray depending on the viewing angle for the surface.
		// Oblique angles require smaller step sizes to achieve
		// more accurate precision for computing displacement.
		// We express the sampling rate as a linear function of the
		// angle between the geometric normal and the view direction ray.
		int numSteps = (int) mix( nMaxSamples, nMinSamples, dot( fin.vView, fin.vNormal ) );

		// Intersect the view ray with the height field profile along
		// the direction of the parallax offset ray (computed in the
		// vertex shader. Note that the code is designed specifically
		// to take advantage of the dynamic flow control constructs in HLSL
		// and is very sensitive to the specific language syntax.
		// When converting to other examples, if still want to use dynamic
		// flow control in the resulting assembly shader, care must be
		// applied.
		// In the below steps we approximate the height field profile
		// as piecewise linear curve. We find the pair of endpoints
		// between which the intersection between the height field
		// profile and the view ray is found and then compute line segment
		// intersection for the view ray and the line segment formed by
		// the two endpoints. This intersection is the displacement
		// offset from the original texture coordinate.

		float currHeight = 0.0;
		float stepSize = 1.0 / (float) numSteps;
		float prevHeight = 1.0;
		float nextHeight = 0.0;
		int stepIndex = 0;
		vec2 vTexOffsetPerStep = stepSize * fin.vParallaxTang;
		vec2 vTexCurrentOffset = texCoord;
		float currentBound = 1.0;
		float parallaxAmount = 0.0;
		vec2 pt1 = 0;
		vec2 pt2 = 0;
		vec2 texOffset2 = 0;

		while ( nStepIndex < numSteps )
		{
			vTexCurrentOffset -= vTexOffsetPerStep;

			// Sample height map which in this case is stored in the
			// alpha channel of the normal map
			currHeight = textureGrad( normalMap, texCurrentOffset, dx, dy ).a;
			currentBound -= stepSize;

			if ( fCurrHeight > fCurrentBound )
			{
				pt1 = vec2( currentBound, currHeight );
				pt2 = vec2( currentBound + stepSize, prevHeight );
				texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;
				nStepIndex = nNumSteps + 1;
			}
			else
			{
				stepIndex++;
				fPrevHeight = fCurrHeight;
			}
		} // End of while ( stepIndex < numSteps )
		float delta2 = pt2.x - pt2.y;
		float delta1 = pt1.x - pt1.y;

		parallaxAmount = (pt1.x * delta2 - pt2.x * delta1 ) / ( delta2 - delta1 );
		vec2 parallaxOffset = fin.vParallaxTang * (1 - parallaxAmount );

		// The computed texture offset for the displaced point
		// on the pseudo-extruded surface:
		vec2 texSampleBase = texCoord - parallaxOffset;
		texSample = texSampleBase;

		// Lerp to bump mapping only if we are in between,
		// transition section:
		cLODColoring = vec4( 1, 1, 1, 1 );
		if ( mipLevel > (float)(nLODThreshold - 1) )
		{
			// Lerp based on the fractional part:
			mipLevelFrac = modf( mipLevel, mipLevelInt );
			if ( bVisualizeLOD )
			{
				// For visualizing: lerping from regular POM-
				// resulted color through blue color for transition layer:
				cLODColoring = vec4( 1, 1, max(1, 2 * mipLevelFrac), 1 );
			}
			// Lerp the texture coordinate from parallax occlusion
			// mapped coordinate to bump mapping smoothly based on
			// the current mip level:
			texSample = mix( texSampleBase, texCoord, mipLevelFrac );

		} // End of if ( mipLevel > threshold - 1 )

		vec2 lightRayTS = vLightTang.xy * fHeightMapRange;
		// Compute the soft blurry shadows taking into account
		// self-occlusion for features of the height field:

		float sh0 = texturGrad( normalMap, texSampleBase, dx, dy ).a;
		float shA = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.88, dx, dy ).a - sh0 - 0.88 ) * 1 * shadowSoftening;
		float sh9 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.77, dx, dy ).a - sh0 - 0.77 ) * 2 * shadowSoftening;
		float sh8 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.66, dx, dy ).a - sh0 - 0.66 ) * 4 * shadowSoftening;
		float sh7 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.55, dx, dy ).a - sh0 - 0.55 ) * 6 * shadowSoftening;
		float sh6 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.44, dx, dy ).a - sh0 - 0.44 ) * 8 * shadowSoftening;
		float sh5 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.33, dx, dy ).a - sh0 - 0.33 ) * 10 * shadowSoftening;
		float sh4 = (texturGrad( normalMap, texSampleBase + lightRayTS
				* 0.22, dx, dy ).a - sh0 - 0.22 ) * 12 * shadowSoftening;

		// Compute the actual shadow strength:
		occlusionShadow = 1 - max( max( max( max( max( max( shA, sh9 ),
								   sh8 ), sh7 ), sh6 ), sh5 ), sh4 );
		// The previous computation over brightens the image, let's adjust for that:
		occlusionShadow = occlusionShadow * 0.6 + 0.4;

	} // End of if ( mipLevel <= (float) nLODThreshold )

	// Compute resulting color for the pixel:
	colour = ComputeIllumination( texSample, vLightTang,
										vViewTang, occlusionShadow );

	if ( bVisualizeLOD )
	{
		colour *= cLODColoring;
	}

	// Visualize currently computed mip level, tinting the color blue
	// if we are in the region outside of the threshold level
	if ( bVisualizeMipLevel )
	{
		colour = mipLevel.xxxx;
	}

	// If using HDR rendering, make sure to tonemap the result color
	// prior to outputting it. But since this example isn't doing that,
	// we just output the computed result color here:
}
