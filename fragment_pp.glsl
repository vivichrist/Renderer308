#version 330
#pragma optimize(off)
#pragma debug(on)

layout(location = 1) out vec4 FBColor;
layout(location = 2) out vec4 FBSpec;

in vec2 texcoord;

uniform sampler2D colour;
uniform sampler2D spec;

uniform vec2 pixelSize;
uniform int isVert;
const mat2 rot90 = mat2( 0, 1, -1, 0 );

void main()
{
    // viv's code start
    vec4 bloom = vec4(0);
    vec4 composite = vec4(0);
    vec2 shift = vec2( pixelSize.x * 0.5, pixelSize.y * 1.5 );
    vec2 shift2 = vec2( 0, pixelSize.y * 2.0 );
    if ( isVert == 1 )
    {
      shift = rot90 * shift;
      shift2 = rot90 * shift;
    }
    composite += 8.0 * texture( colour, texcoord );
    composite += 4.0 * texture( colour, texcoord + shift );
    composite += 4.0 * texture( colour, texcoord + (-shift) );
    composite += 2.0 * texture( colour, texcoord + shift + shift2 );
    composite += 2.0 * texture( colour, texcoord + (-(shift + shift2)) );
    composite += texture( spec, texcoord + shift + shift2 + shift2 );
    composite += texture( spec, texcoord + (-(shift + shift2 + shift2)) );

    bloom += 12.0 * texture( spec, texcoord );
    bloom += 8.0 * texture( spec, texcoord + shift );
    bloom += 8.0 * texture( spec, texcoord + (-shift) );
    bloom += 4.0 * texture( spec, texcoord + shift + shift2 );
    bloom += 4.0 * texture( spec, texcoord + (-(shift + shift2)) );
    bloom += 2.0 * texture( spec, texcoord + shift + 2.0 * shift2 );
    bloom += 2.0 * texture( spec, texcoord + (-(shift + 2.0 * shift2 )) );
    bloom += texture( spec, texcoord + shift + 3.0 * shift2 );
    bloom += texture( spec, texcoord + (-(shift + 3.0 * shift2 )) );

    // viv's code end
    FBColor = vec4( composite.xyz / 20.0 , 1 );
    FBSpec = vec4( bloom.xyz / 38.0 , 1 );
}