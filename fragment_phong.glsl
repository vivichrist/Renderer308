// fragment shader for simple Phong Lighting model
#version 330

out vec4 FBColor;

smooth in vec2 vUV;
smooth in vec3 vNormal;
smooth in vec3 vView;
smooth in vec3 vLightDir;

uniform mat4 mvM;
uniform sampler2D image;

void main()
{
    // Dot product gives us diffuse intensity
	float diff = max(0.0, dot(normalize(vNormal), vLightDir));
	// Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
	FBColor = max( diff, 0.05 ) * texture( image, vUV );

	// Specular Light
	vec3 halfN = normalize(vLightDir - normalize(vView));

	float spec = max(0.0, dot(normalize(vNormal), halfN));

	// If the diffuse light is zero, donât even bother with the pow function
	if ( diff > 0 )
	{
		float fSpec = pow(spec, 128.0);
		FBColor.rgb += vec3(fSpec, fSpec, fSpec);
	}
}