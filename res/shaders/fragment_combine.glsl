#version 330

out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D blurColour;
uniform sampler2D blurSpec;

uniform float dof;

void main()
{
    // viv's code start
    vec4 c = texture( colour, texcoord );
    float d = texture( depth, texcoord ).r;
    float depth = 1.0 - (12.0 * d - 11.0);
    float dofOffset = clamp( 1.5 * (abs( log(depth) - log(dof) / max( depth, dof ) ) ) - 0.5
                                 , 0.0, 1.0);

    vec4 bloom = texture( blurSpec, texcoord );
    vec4 composite = texture( blurColour, texcoord );

    // viv's code end
    FBColor = vec4( dofOffset * composite.rgb * 2.0
                    + bloom.rgb
                    + (1.0 - dofOffset) * c.rgb, 1);
//    FBColor = vec4( dofOffset,dofOffset,dofOffset,1 );
}