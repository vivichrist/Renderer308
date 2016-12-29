// fragment shader for simple Phong Lighting model
#version 330

out vec4 FBColor;

in GeoData
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vView;
	vec3 vLightDir;
	noperspective vec3 dists;
} g_in;

uniform sampler2D image;

void main()
{
    // Dot product gives us diffuse intensity
	float diff = max(0.0, dot(normalize(g_in.vNormal), g_in.vLightDir));
	// Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
	float d = min(min( g_in.dists.x, g_in.dists.y), g_in.dists.z );
	float frameMix = exp2(0.25+(-1500)*(1 - gl_FragCoord.z)*d*d);
	FBColor = mix( max( diff, 0.1 ) * texture( image, g_in.vUV ), max( diff, 0.3 ) * vec4( 1,0,0,1 ), frameMix );

	// Specular Light
	vec3 halfway = normalize(g_in.vLightDir - normalize(g_in.vView));

	float spec = max(0.0, dot(normalize(g_in.vNormal), halfway));

	// If the diffuse light is zero, don’t even bother with the pow function
	if ( diff > 0 )
	{
		float fSpec = pow(spec, 128.0);
		FBColor.rgb += vec3(fSpec, fSpec, fSpec);
	}
}