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
    float dofOffset = abs(dof - d)/ max( d, dof );

    vec4 bloom = texture( blurSpec, texcoord );
    vec4 composite = texture( blurColour, texcoord );

    // viv's code end
    FBColor = vec4( composite.rgb * dofOffset * 0.5
                    + bloom.rgb
                    + (1.0 - dofOffset) * c.rgb, 1);
//   FBColor = vec4( composite.xyz,1 );
}