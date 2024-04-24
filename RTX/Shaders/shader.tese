#version 450 

layout (triangles, fractional_even_spacing, cw) in;

layout(location = 0) in vec3 inColor[];
layout(location = 0) out vec3 outColor;

layout(location = 1) in vec3 inPos[];
layout(location = 1) out vec3 outPos;

layout(location = 2) in vec3 inNorm[];
layout(location = 2) out vec3 outNorm;

layout(binding=0) uniform Transform {
    mat4 M;
    mat4 V;
    mat4 P;
	vec3 cameraPos;
} transform;

void main()
{

	const vec4 b300 = gl_in[0].gl_Position;
	const vec4 b030 = gl_in[1].gl_Position;
	const vec4 b003 = gl_in[2].gl_Position;

	vec4 b210 = (2*b300 + b030 - (b030-b300)*vec4(inNorm[0], 1.0f))/3.0;
	vec4 b120 = (2*b030 + b300 - (b300-b030)*vec4(inNorm[1], 1.0f))/3.0;
	vec4 b021 = (2*b030 + b003 - (b003-b030)*vec4(inNorm[1], 1.0f))/3.0;
	vec4 b012 = (2*b003 + b030 - (b030-b003)*vec4(inNorm[2], 1.0f))/3.0;
	vec4 b102 = (2*b003 + b300 - (b300-b003)*vec4(inNorm[2], 1.0f))/3.0;
	vec4 b201 = (2*b300 + b003 - (b003-b300)*vec4(inNorm[0], 1.0f))/3.0;

	vec4 E = (b210+b120+b021+b012+b102+b201)/6.0;
	vec4 V = (b300+b030+b003)/3.0;

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    float uPow3 = pow(u, 3);
    float vPow3 = pow(v, 3);
    float wPow3 = pow(w, 3);
    float uPow2 = pow(u, 2);
    float vPow2 = pow(v, 2);
    float wPow2 = pow(w, 2);

	vec4 b111 = E+(E-V)/2.0;

    vec4 p = b300 * wPow3 + b030 * uPow3 + b003 * vPow3 + b210 * 3.0 * wPow2 * u + b120 * 3.0 * w * uPow2 + b201 * 3.0 * wPow2 * v + b021 * 3.0 * uPow2 * v + b102 * 3.0 * w * vPow2 
			+ b012 * 3.0 * u * vPow2 + b111 * 6.0 * w * u * v;

	//vec4 p = vec4((E+(E-V)/2.0).xyz, 1.0f);  
	//vec4 p = vec4(gl_TessCoord.x * gl_in[0].gl_Position.xyz + gl_TessCoord.y * gl_in[1].gl_Position.xyz + gl_TessCoord.z*gl_in[2].gl_Position.xyz, 1.0);

	gl_Position = transform.P * transform.V * transform.M * p;

	outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];

	outPos = (transform.M * p).xyz;

	outNorm = gl_TessCoord.x * inNorm[0] + gl_TessCoord.y * inNorm[1] + gl_TessCoord.z * inNorm[2];

}
