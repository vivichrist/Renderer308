// fragment shader for simple Phong Lighting model
#version 330

smooth in vec3 vLightDir;
smooth in vec2 vUV;
smooth in vec3 vNormal;

uniform sampler2D image;

out vec4 FBColor;

void main()
{
    // Dot product gives us diffuse intensity
	float diff = max(0.0, dot(normalize(vNormal), normalize(vLightDir)));
	// Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
	FBColor = max( diff, 0.05 ) * texture( image, vUV );

	// Specular Light
	vec3 reflection = normalize(reflect(-normalize(vLightDir), normalize(vNormal)));
	float spec = max(0.0, dot(normalize(vNormal), reflection));
	// If the diffuse light is zero, don’t even bother with the pow function
	if(diff != 0)
	{
		float fSpec = pow(spec, 128.0);
		FBColor.rgb += vec3(fSpec, fSpec, fSpec);
	}
}