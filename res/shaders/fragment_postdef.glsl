// fragment shader for simple Phong Lighting model
#version 330

out vec4 FBColor;

in vec3 position;

uniform sampler2D depth;
uniform sampler2D colour;
uniform sampler2D normal;
uniform sampler2D pos;
uniform sampler2D eye;

void main()
{
    vec3 d = texture( depth, position.xy );
    vec3 c = texture( colour, position.xy );
    vec3 n = texture( normal, position.xy );
    vec3 p = texture( pos, position.xy );
    vec3 e = texture( eye, position.xy );
    FBColor = d + c + n + p + e; 
}
