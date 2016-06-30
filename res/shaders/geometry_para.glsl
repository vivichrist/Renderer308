#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat3 normM; // Matrix to transform normals.
uniform mat3 light;
uniform vec3 viewP;

in VertexData
{
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
} vin[3];

out FragData
{
    smooth vec2 vUV;
    smooth vec3 vNormal;
    smooth vec3 vView;
    smooth vec3 vTangentLightPos;
	smooth vec3 vTangentFragPos;
	smooth vec3 vTangentView;
	smooth vec3 vTangentNormal;
} gout;

void main()
{
    // T = (u3 − u1)x(p2 − p1) − (u2 − u1)x(p3 − p1)
    //     / (u3 − u1)x(v2 − v1) − (u2 − u1)x(v3 − v1)
    // B = (u2 − u1)x(p3 − p1) − (u3 − u1)x(p2 − p1)
    //     / (u3 − u1)x(v2 − v1) − (u2 − u1)x(v3 − v1)
    float u3u1 = vin[2].vUV.y - vin[0].vUV.y;
    float u2u1 = vin[1].vUV.y - vin[0].vUV.y;
    
    vec3 p2p1 = vin[1].vView - vin[0].vView;
    vec3 p3p1 = vin[2].vView - vin[0].vView;
    
    float over = 1.0 / (u3u1 * (vin[1].vUV.x - vin[0].vUV.x)
                      - u2u1 * (vin[2].vUV.x - vin[0].vUV.x));

    vec3 tangent = (u3u1 * p2p1 - u2u1 * p3p1) * over;
    vec3 bitangent = (u2u1 * p3p1 - u3u1 * p2p1) * over;
    vec3 normal = cross( tangent, bitangent );

    // Tangent Space
    vec3 T = normalize(normM * tangent);
    vec3 B = normalize(normM * bitangent);
    vec3 N = normalize(normM * normal);
    mat3 TBN = mat3(T, B, N);
    
    vec3 tView     = TBN * normM * viewP;
    vec3 tLightPos = TBN * normM * light[0];
    
    int i;
    for ( i = 0; i<3; ++i )
	{
		gout.vUV = vin[i].vUV;
		gout.vNormal = vin[i].vNormal;
		gout.vView = vin[i].vView;
		gout.vTangentLightPos = tLightPos;
		gout.vTangentView = tView;
		gout.vTangentFragPos = TBN * vin[i].vView.xyz;
		gout.vTangentNormal = normalize(TBN * vin[i].vNormal);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
    EndPrimitive();
}