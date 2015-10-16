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

uniform mat4 mvM;
uniform sampler2D image;

void main()
{
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vNormal), vLightDir));

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    colour = max( diff, 0.2 ) * texture( image, vUV );// + vec4(0.3,0.3,0.3,1);

    // Specular Light
    vec3 halfway = normalize(vLightDir - normalize(vView));

    float spec = max(0.0, dot(normalize(vNormal), halfway));
    eye = vec4(0);
    // If the diffuse light is zero, don’t even bother with the pow function
    if ( diff > 0 )
    {
        float fSpec = pow(spec, 128.0);
        eye = vec4(fSpec, fSpec, fSpec, 1);
    }

    normal = vec4( vNormal, 1 );

    // colour = vec4(1.0, 0.0, 0.0, 1.0);
}