// fragment shader for simple Phong Lighting model
#version 330
#define MAX_LIGHTS 10

out vec4 FBColor;

in VertexData {
	smooth vec2 vUV;
	smooth vec3 vNormal;
	smooth vec3 vView;
} fin;

uniform mat4 mvM;
uniform vec4 material; // intensities, shininess is w
uniform sampler2D image;

uniform struct Light {
	vec4 pos;
	vec4 spec; // specular intensities
	vec4 att; // attenuation coefficients, ambient coefficient is w;
	vec4 coneDir; // cone direction, cone angle is w;
} allLights[MAX_LIGHTS];

uniform int numLights;

vec3 lighting( Light l, vec3 diffuse, vec3 normal, vec3 pos, vec3 eye )
{
    vec3 lightDir; // vector from surface to the light
    float att = 1.0; // attenuation

    if ( l.pos.w == 0.0 ) // directional light
    {
        lightDir = normalize(l.pos.xyz);
        att = 1.0; // no attenuation for directional lights
    } 
    else
    {   // point light
        vec4 lgt4 = mvM * l.pos;
        lightDir = (lgt4.xyz / lgt4.w) - pos;
        float dist = length(lightDir);
        lightDir = normalize(lightDir);
        // spotlight cone
        float inside = degrees(acos(dot(-eye, normalize(l.coneDir.xyz))));
        if ( inside > l.coneDir.w )
        {
            att = 0.0;
        }
        else att = 1.0/(l.att.x + (l.att.y*dist) + (l.att.z*dist*dist));
    }
    vec3 mat = diffuse.rgb * material.xyz; // matte colour
    
    // ambient
    vec3 ambient = l.att.w * mat;

    // diffuse
    float dI = max(0.0, dot(normal, lightDir));
    vec3 diff = dI * mat;
    // backfacing from the light?
	if ( dI <= 0.0 ) return ambient + att * diff;

    //specular
    float sI = pow(max(0.0, dot(normalize(normal), halfN))), material.w);
    vec3 spec = sI * l.spec * material.xyz;

    //output color (no gamma correction)
    return ambient + att * ( diff + spec );
}

void main()
{
    vec3 diffColor = texture( image, fin.vUV );
    for ( int i = 0; i<numLights; ++i )
        FBColor.rgb += lighting( allLights[i], diffColor, fin.vNormal
                                            , gl_Position, fin.vView );
}