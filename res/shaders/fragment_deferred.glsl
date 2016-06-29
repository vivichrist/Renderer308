// fragment shader for deferred rendering
#version 330
#pragma optimize(off)
#pragma debug(on)

// if we need to write to the depth buffer, depth_greater means
// 'You will only make the depth larger, compared to gl_FragDepth.z​'
//layout (depth_greater) out float gl_FragDepth;

layout(location = 0) out vec3 colour;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;
layout(location = 3) out vec3 eye;
layout(location = 4) out vec3 refl;

in VertexData {
    smooth vec3 vPos;
	smooth vec2 vUV;
	smooth vec3 vNormal;
	smooth vec3 vView;
} fin;

uniform mat4 mvM;
uniform mat3 normM;

uniform sampler2D image;
uniform sampler2D normalMap;
uniform samplerCube eMap;

void main()
{
	colour = texture( image, texCoord );
	normal = fin.vNormal;
	if ( hasNMap > 0 ) normal = normal * texture( normalMap, texCoord );
	texCoord = fin.vUV;
	eye = fin.vView;
	refl = fin.vPos;
}
