#version 330

out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D eye;

uniform vec2 pixelSize;
uniform float dof;
const float pi = 3.14159265359, pi_2 = 1.57079632679, pi_4 = 0.78539816339;

void main()
{
    vec4 c = texture( colour, texcoord );
    vec4 e = texture( eye, texcoord );
    float d = texture( depth, texcoord ).r;
    // viv's code start
    float dofOffset = abs( d - dof ) / max( d, dof );
    vec2 halfpix = pixelSize * 0.5;
    mat2 rot90 = mat2( 0, 1, -1, 0 );
    vec4 composite = c;
    vec4 bloom = e;
    vec2 blockpos = vec2( pixelSize.x * 1.5, pixelSize.y * 0.5 );
    vec2 pos1 = blockpos;
    vec2 pos2 = blockpos + vec2( 0.0, 2.0 * pixelSize.y );
    vec2 pos3 = blockpos + vec2( 2.0 * pixelSize.x, 0.0 );
    vec2 pos4 = blockpos + vec2( 2.0 * pixelSize.x, 2.0 * pixelSize.y );
    vec2 pos5 = blockpos + vec2( 4.0 * pixelSize.x, 2.0 * pixelSize.y );
    vec2 pos6 = blockpos + vec2( 2.0 * pixelSize.x, 4.0 * pixelSize.y );
    for ( float k = 0.0; k < 4.0; k+=1.0 )
    {
        composite += texture( colour, texcoord.xy + pos1 );
        bloom += textureLod( eye, texcoord.xy + pos1, 4 );
        composite += 0.5 * texture( colour, texcoord.xy + pos2  );
        bloom += 0.5 * textureLod( eye, texcoord.xy + pos2, 4 );
        composite += 0.25 * texture( colour, texcoord.xy + pos3 );
        bloom += 0.25 * textureLod( eye, texcoord.xy + pos3, 4 );
        composite += 0.125 * texture( colour, texcoord.xy + pos4 );
        bloom += 0.125 * textureLod( eye, texcoord.xy + pos4, 4 );
        bloom += 0.06 * textureLod( eye, texcoord.xy + pos5, 4 );
        bloom += 0.06 * textureLod( eye, texcoord.xy + pos6, 4 );
        pos1 = rot90 * pos1;
        pos2 = rot90 * pos2;
        pos3 = rot90 * pos3;
        pos4 = rot90 * pos4;
        pos5 = rot90 * pos5;
        pos6 = rot90 * pos6;
    }
    composite *= 0.1;
    bloom *= 0.5;
    // viv's code end
	FBColor = vec4( bloom.xyz , 1);
	// FBColor = texture( depth, texcoord ).rrra;
}