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
    float dofOffset = abs( (1.0 + d) - (1.0 + dof) );
    vec2 halfpix = pixelSize * 0.5;
    mat2 rot90 = mat2( 0, 1, -1, 0 );
    mat2 rot45 = mat2( cos(pi_4), -sin(pi_4), cos(pi_4), sin(pi_4) );
    mat2 rot22 = mat2( cos(pi_4/2), -sin(pi_4/2), cos(pi_4/2), sin(pi_4/2) );
    mat2 rot11 = mat2( cos(pi_4/4), -sin(pi_4/4), cos(pi_4/4), sin(pi_4/4) );
    vec4 composite = vec4(0);
    vec4 bloom = vec4(0);
    vec2 blockpos = vec2( pixelSize.x * 1.5, pixelSize.y * 0.5 );
    vec2 pos = blockpos;
    for ( float k = 0.0; k < 4.0; k+=1.0 )
    {
        composite += texture( colour, texcoord.xy + pos );
        pos = rot90 * pos;
    }
    pos = blockpos + vec2( 0.0, 2.0 * pixelSize.y );
    for ( float k = 0.0; k < 4.0; k+=1.0 )
    {
        composite += texture( colour, texcoord.xy + pos );
        pos = rot90 * pos;
    }
    pos = blockpos + vec2( 2.0 * pixelSize.x, 0.0 );
    for ( float k = 0.0; k < 4.0; k+=1.0 )
    {
        bloom += texture( eye, texcoord.xy + pos );
        pos = rot90 * pos;
    }
    pos = blockpos + vec2( 2.0 * pixelSize.x, 2.0 * pixelSize.y );
    for ( float k = 0.0; k < 18.0; k+=1.0 )
    {
        bloom += texture( eye, texcoord.xy + pos, 3 );
        pos = rot90 * pos;
    }
    composite *= 0.05;
    bloom *= 0.05;
    // viv's code end
	FBColor = vec4( composite.xyz * dofOffset + bloom.xyz* dofOffset + ((1 - dofOffset) * c.xyz), 1);
	// FBColor = texture( depth, texcoord ).rrra;
}