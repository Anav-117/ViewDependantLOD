#version 450

layout(location = 0) in vec3 pos_attrib;

layout(location = 0) out vec3 fragColor;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(binding=0) uniform Transform {
    mat4 M;
    mat4 V;
    mat4 P;
    vec3 cameraPos;
} transform;

void main() {
    gl_Position = vec4(pos_attrib, 1.0);
    fragColor = colors[int(gl_VertexIndex) % 3];
}