#version 330

out vec4 FBColor;

in vec2 texcoord;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;
uniform sampler2D eye;

void main()
{
    vec4 d = texture( depth, texcoord );
    vec4 c = texture( colour, texcoord );
    vec4 n = texture( normal, texcoord );
    vec4 e = texture( eye, texcoord );
    FBColor = e;
    //FBColor = d + c + n + p + e; 
    //FBColor = vec4(d.r,d.r,d.r,1);
}