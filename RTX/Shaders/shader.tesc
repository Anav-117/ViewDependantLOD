#version 450

layout (vertices = 3) out;  //number of output verts of the tess. control shader
layout(location = 0) in vec3 inColor[];
layout(location = 0) out vec3 outColor[];

layout(location = 1) in vec3 inPos[];
layout(location = 1) out vec3 outPos[];

layout(location = 2) in vec3 inNorm[];
layout(location = 2) out vec3 outNorm[];

layout(binding=0) uniform Transform {
    mat4 M;
    mat4 V;
    mat4 P;
	vec3 cameraPos;
} transform;

void main()
{

	//Pass-through: just copy input vertices to output
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	vec3 patchCenter = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;

	float dist = 100.0*exp(-length(patchCenter - transform.cameraPos)* 0.5);

	//set tessellation levels
	gl_TessLevelOuter[0] = dist;
	gl_TessLevelOuter[1] = dist;
	gl_TessLevelOuter[2] = dist;
	//gl_TessLevelOuter[3] = 20.0;

	gl_TessLevelInner[0] = dist;
	//gl_TessLevelInner[1] = 2.0;

	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outPos[gl_InvocationID] = inPos[gl_InvocationID];
	outNorm[gl_InvocationID] = inNorm[gl_InvocationID];
}
