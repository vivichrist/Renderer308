// fragment shader for simple Phong Lighting model
#version 330

out vec4 FBColor;

smooth in vec2 vUV;
smooth in vec3 vNormal;
smooth in vec3 vView;
smooth in vec3 vLightDir;

uniform mat4 mvM;
uniform sampler2D image;
uniform vec4 material; // intensities, shininess is w

uniform int numLights;
#define MAX_LIGHTS 10
uniform struct Light {
	vec4 pos;
	vec4 spec; // color of specular highlights
	vec4 att; // attenuation coefficients, ambient coefficient is w;
	vec4 coneDir; // cone direction, cone angle is w;
} allLights[MAX_LIGHTS];

vec3 ApplyLight(Light light, vec3 diffuse, vec3 normal, vec3 pos, vec3 eye)
{
    vec3 lightDir; // vector from surface to the light
    float att = 1.0; // attenuation

    if(light.pos.w == 0.0) // directional light
    {
        lightDir = normalize(light.pos.xyz);
        att = 1.0; // no attenuation for directional lights
    } else {
        //point light
        lightDir = normalize( light.pos.xyz - pos );
        float dist = length(light.pos.xyz - pos);

        //cone restrictions (affects attenuation)
        float inside = degrees(acos(dot(-eye, normalize(light.coneDir))));
        if ( inside > light.coneDir.w )
        {
            att = 0.0;
        }
        else att = 1.0/(light.att.x + (light.att.y*dist) + (light.att.z*dist*dist));
    }

    //ambient
    vec3 ambient = light.att.w * diffuse.rgb * material.xyz;

    //diffuse
    float dI = max(0.0, dot(normal, lightDir));
    vec3 diff = dI * diffuse.rgb * material.xyz;
	if ( dI <= 0.0 ) return ambient + att * diff;

    //specular
    float fSpec = pow(max(0.0, dot(normalize(vNormal), halfN))), material.w);
    vec3 spec = fSpec * light.spec * material.xyz;

    //output color (no gamma correction)
    return ambient + att * ( diff + spec );
}

void main()
{
    // Diffuse intensity
	float diff = max(0.0, dot(normalize(vNormal), normalize(vLightDir)));

	// Light attenuation
	float dist = length( vLightDir );
	float attenAmount = 1.0/(k.x + (k.y*dist) + (k.z*dist*dist));

	// Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
	FBColor = max( diff, 0.05 ) * texture( image, vUV ) * attenAmount;

	// Specular Light
	vec3 halfN = normalize(vLightDir - normalize(vView));

	float spec = max(0.0, dot(normalize(vNormal), halfN));

	// If the diffuse light is zero, don't even bother with the pow function
	if ( diff > 0 )
	{
		float fSpec = pow(spec, 128.0);
		FBColor.rgb += vec3(fSpec, fSpec, fSpec);
	}
}