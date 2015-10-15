#version 330 core
// #extension GL_EXT_geometry_shader4 : enable
#pragma optimize(off)
#pragma debug(on)

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

smooth out vec2 texCoord;

void main(void)
{
	gl_Position = vec4( -1, -1, 1, 1 );
	texCoord = vec2( 0, 0 );
	EmitVertex();
	gl_Position = vec4( 1, 1, 1, 1 );
	texCoord = vec2( 1, 1 );
	EmitVertex();
	gl_Position = vec4( -1, 1, 1, 1 );
	texCoord = vec2( 0, 1 );
	EmitVertex();
	EndPrimitive();
	gl_Position = vec4( -1, -1, 1, 1 );
	texCoord = vec2( 0, 0 );
	EmitVertex();
	gl_Position = vec4( 1, -1, 1, 1 );
	texCoord = vec2( 1, 0 );
	EmitVertex();
	gl_Position = vec4( 1, 1, 1, 1 );
	texCoord = vec2( 1, 1 );
	EmitVertex();
	EndPrimitive();
}