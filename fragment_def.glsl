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
uniform sampler2D normalmap;
uniform sampler2D heightmap;

void main()
{
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vNormal), vLightDir)) * 4;

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    colour = texture( image, (vUV*4-50)+vec2(1.5,1) );

    // Specular Light
    vec3 halfway = normalize(vLightDir - normalize(vView));

    float spec = max(0.0, dot(normalize(vNormal), halfway));

    // If the diffuse light is zero, don’t even bother with the pow function
    if ( diff > 0 )
    {
        float fSpec = pow(spec, 128.0);
        //colour.rgb += vec3(fSpec, fSpec, fSpec);
    }

    eye = vec4( vView, 1 );
    normal = vec4( vNormal, 1 );

    // colour = vec4(1.0, 0.0, 0.0, 1.0);
}