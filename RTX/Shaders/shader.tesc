#version 450

layout (vertices = 3) out;  //number of output verts of the tess. control shader
layout(location = 0) in vec3 inColor[];
layout(location = 0) out vec3 outColor[];

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

	float dist = 100.0*exp(-length(gl_in[gl_InvocationID].gl_Position.xyz - transform.cameraPos));

	//set tessellation levels
	gl_TessLevelOuter[0] = dist;
	gl_TessLevelOuter[1] = dist;
	gl_TessLevelOuter[2] = dist;
	//gl_TessLevelOuter[3] = 20.0;

	gl_TessLevelInner[0] = dist;
	//gl_TessLevelInner[1] = 2.0;

	outColor[gl_InvocationID] = inColor[gl_InvocationID];
}
