#version 330

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
    if ( isVert == 0 )
    {
      shift = rot90 * shift;
      shift2 = rot90 * shift;
    }
    composite += texture( colour, texcoord );
    composite += 0.5 * texture( colour, texcoord + shift );
    composite += 0.5 * texture( colour, texcoord + (-shift) );
    composite += 0.25 * texture( colour, texcoord + shift + shift2 );
    composite += 0.25 * texture( colour, texcoord + (-(shift + shift2)) );

    bloom += texture( spec, texcoord );
    bloom += 0.5 * texture( spec, texcoord + shift );
    bloom += 0.5 * texture( spec, texcoord + (-shift) );
    bloom += 0.25 * texture( spec, texcoord + shift + shift2 );
    bloom += 0.25 * texture( spec, texcoord + (-(shift + shift2)) );
    bloom += 0.125 * texture( spec, texcoord + shift + shift2 + shift2 );
    bloom += 0.125 * texture( spec, texcoord + (-(shift + shift2 + shift2)) );

    // viv's code end
    FBColor = vec4( composite.xyz, 1 );
    FBSpec = vec4( bloom.xyz, 1 );
    //FBbla = vec4( 1, 0, 0, 1 );
}