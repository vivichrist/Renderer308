#version 330
#extension GL_EXT_geometry_shader4 : enable

uniform mat4 mvM[6];

in VertexData {
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
} vin[];

out VertexData {
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
    flat mat4 mvM;
} gout;

void main(void)
{
    int i, layer;
    for ( layer = 0; layer < 6; ++layer )
    {
        mNorm = inverse( transpose( mvM[layer] ) );
        gl_Layer = layer;
        for ( i = 0; i < 3; i++ )
        {
            // Get surface normal in eye coordinates
		    gout.vNormal = normalize( mNorm * vin[i].vNormal);
		    // Get vertex position in eye coordinates
		    vec4 pos4 = mvM[layer] * vec4( gl_Position, 1 );
		    gout.vView = pos4.xyz / pos4.w;
		    // transform the geometry!
		    gl_Position = projM * pos4;
            gout.mvM = mvM[layer];
            EmitVertex();
        }
        EndPrimitive();
    }
}