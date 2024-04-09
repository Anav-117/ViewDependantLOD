#version 450 

layout (triangles, fractional_even_spacing, cw) in;

layout(location = 0) in vec3 inColor[];
layout(location = 0) out vec3 outColor;


void main()
{

	const vec4 p0 = gl_in[0].gl_Position;
	const vec4 p1 = gl_in[1].gl_Position;
	const vec4 p2 = gl_in[2].gl_Position;

	vec4 p = vec4(gl_TessCoord.x * gl_in[0].gl_Position.xyz + gl_TessCoord.y * gl_in[1].gl_Position.xyz + gl_TessCoord.z*gl_in[2].gl_Position.xyz, 1.0);

	gl_Position = p;

	outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];

}
