#version 330
#extension GL_EXT_geometry_shader4 : enable

uniform mat4 mvM[6];

out VertexData {
    smooth out vec2 vUV;
    smooth out vec3 vNormal;
    smooth out vec3 vView;
} vin[];

out VertexData {
    smooth out vec2 vUV;
    smooth out vec3 vNormal;
    smooth out vec3 vView;
    noperspective out mat4 mvM;
} gout;

void main(void)
{
    int i, layer;
    for ( layer = 0; layer < 6; ++layer )
    {
        gl_Layer = layer;
        for (i = 0; i < 3; i++)
        {
            gl_Position = gl_PositionIn[i];
            gout.vUV = vin[i].vUV;
            gout.vNormal = vin[i].vNormal;
            gout.vView = vin[i].vView;
            gout.mvM = mvM[i];
            EmitVertex();
        }
        EndPrimitive();
    }
}