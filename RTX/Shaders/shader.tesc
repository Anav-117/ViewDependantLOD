#version 450

layout (vertices = 3) out;  //number of output verts of the tess. control shader
layout(location = 0) in vec3 inColor[];
layout(location = 0) out vec3 outColor[];

void main()
{

	//Pass-through: just copy input vertices to output
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	//set tessellation levels
	gl_TessLevelOuter[0] = 1.0;
	gl_TessLevelOuter[1] = 3.0;
	gl_TessLevelOuter[2] = 10.0;

	gl_TessLevelInner[0] = 3.0;

	outColor[gl_InvocationID] = inColor[gl_InvocationID];
}