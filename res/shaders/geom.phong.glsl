#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in VertexData
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vView;
	vec3 vLightDir;
} v_in[3];

out GeoData
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vView;
	vec3 vLightDir;
	noperspective vec3 dists;
} g_out;

void main()
{
	vec3 d;
	d.x = length(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz))
						/ length(gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);
	d.y = length(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz))
						/ length(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	d.z = length(cross(gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz))
						/ length(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz);
	for(int i = 0; i < 3; ++i)
	{
		g_out.dists = vec3(0);
		g_out.dists[i] = d[i];
		g_out.vUV = v_in[i].vUV;
		g_out.vNormal = v_in[i].vNormal;
		g_out.vView = v_in[i].vView;
		g_out.vLightDir = v_in[i].vLightDir;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}