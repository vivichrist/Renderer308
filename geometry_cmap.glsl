#version 330
#extension GL_EXT_geometry_shader4 : enable

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

uniform mat4 mvM[6], projM;
uniform mat3 normM[6];

in VertexData {
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
} vin[3];

out VertexData {
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
    flat mat4 mvM;
    flat mat3 normM;
} gout;

void main(void)
{
    int i, layer;
    for ( layer = 0; layer < 6; ++layer )
    {
        gl_Layer = layer;
        for ( i = 0; i < 3; i++ )
        {
            // Get surface normal in eye coordinates
		    gout.vNormal = normalize( normM[layer] * vin[i].vNormal);

		    // Get vertex position in eye coordinates
		    vec4 pos4 = mvM[layer] * gl_Position;
		    gout.vView = pos4.xyz / pos4.w;

		    // transform the geometry!
		    gl_Position = projM * pos4;
		    
            gout.normM = normM[layer];
            gout.mvM = mvM[layer];
            
            EmitVertex();
        }
        EndPrimitive();
    }
}