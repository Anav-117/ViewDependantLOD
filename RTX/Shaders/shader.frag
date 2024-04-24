#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec3 normal;

layout(binding=0) uniform Transform {
    mat4 M;
    mat4 V;
    mat4 P;
	vec3 cameraPos;
} transform;

layout(location = 0) out vec4 outColor;

vec3 lightPos = vec3(0.0, 20.0, 0.0);

const float pi = 3.141592653589793238462643;

vec3 orrenNayar() {

    float ndotl = max(0, dot(normalize(lightPos - pos), normalize(normal)));
    float ndotv = max(0, dot(normalize(transform.cameraPos - pos), normalize(normal)));

    float alpha = max(acos(ndotl), acos(ndotv));
    float beta = min(acos(ndotl), acos(ndotv));
    
    float roughness = 0.9;
    float r_sqr = roughness*roughness;

    vec3 onFraction = vec3(r_sqr/(r_sqr+0.33), r_sqr/(r_sqr+0.13), r_sqr/(r_sqr+0.09));

    vec3 on = vec3(1.0, 0.0, 0.0) + vec3(-0.5, 0.17, 0.45) * onFraction;

    float oren_nayar_s = clamp(dot(normalize(lightPos - pos), normalize(transform.cameraPos - pos)), 0.0, 1.0) - clamp(ndotl, 0.0, 1.0) * clamp(ndotv, 0.0, 1.0);

    oren_nayar_s /= mix(max(clamp(ndotl, 0.0, 1.0), clamp(ndotv, 0.0, 1.0)), 1, step(oren_nayar_s, 0));

    vec3 diffuseColor = vec3(1.0);

    return clamp(ndotl, 0.0, 1.0) * (on.x + diffuseColor * on.y + on.z * oren_nayar_s) * diffuseColor;

}

vec3 minneart() {

    float ndotl = max(0, dot(normalize(lightPos - pos), normalize(normal)));
    float ndotv = max(0, dot(normalize(transform.cameraPos - pos), normalize(normal)));

    float roughness = 0.9;

    return clamp(ndotl * pow(ndotl*ndotv, roughness), 0.0, 1.0) * vec3(1.0);

}

vec3 banks() {
    
    vec3 tangent = normalize(cross(normalize(normal), vec3(0.0, 1.0, 0.0)));
    float ldott = max(0, dot(normalize(lightPos - pos), normalize(tangent)));
    float vdott = max(0, dot(normalize(transform.cameraPos - pos), normalize(tangent)));

    float roughness = 0.5;

    vec3 diffuse = sqrt(1 - pow(max(ldott, 0.0), 2)) * vec3(0.0);
    vec3 specular = pow(sqrt(1 - pow(max(ldott, 0.0), 2)) * sqrt(1 - pow(max(vdott, 0.0), 2)) - ldott*vdott, 2.0) * vec3(1.0);

    return diffuse+specular;

}

vec3 ward() {
    
    float roughness1 = 0.75;
    float r1_sqr = roughness1*roughness1;
    float roughness2 = 0.25;
    float r2_sqr = roughness2*roughness2;

    float ndotl = max(0.00001, dot(normalize(lightPos - pos), normalize(normal)));
    float ndotv = max(0.00001, dot(normalize(transform.cameraPos - pos), normalize(normal)));

    vec3 tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    vec3 h = normalize((lightPos - pos) + (transform.cameraPos - pos));

    float phi = acos(dot(normalize(h - dot(h, normal))*normal, tangent));

    float gamma = acos(dot(h, normalize(normal)));

    vec3 specular = exp(-1*pow(tan(gamma), 2.0) * (pow(cos(phi), 2.0)/r1_sqr + pow(sin(phi), 2.0)/r2_sqr)) / (4 * pi * roughness1*roughness2 * sqrt(ndotl*ndotv)) * vec3(1.0);

    return specular;

}

void main() {

    //vec3 color = orrenNayar();
    //vec3 color = minneart();
    //vec3 color = banks();
    vec3 color = ward();

    
    outColor = vec4(color, 1.0);
}