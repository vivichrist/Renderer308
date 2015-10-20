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
smooth in vec3 vLightDir;

uniform mat4 mvM;
uniform sampler2D image;

void main()
{
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vNormal), vLightDir));

    // Multiply intensity by diffuse color, force alpha to 1.0 and add in ambient light
    colour = max( diff, 0.05 ) * (texture( image, vUV ));

    // Specular Light
    vec3 halfway = normalize(vLightDir - normalize(vView));
    float specular = max(0.0, dot(normalize(vNormal), halfway));
    float intensity = colour.x + colour.y + colour.z / 3.0;
    spec = (intensity > 0.9) ? vec4( intensity, intensity, intensity, 1 )
                             : vec4(0,0,0,1);
    // If the diffuse light is zero, don’t even bother with the pow function
    if ( diff > 0 )
    {
        float fSpec = pow(specular, 128.0);
        spec = vec4(fSpec, fSpec, fSpec, 1);
    }

    normal = vec4( vNormal, 1 );
}