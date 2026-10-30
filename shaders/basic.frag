#version 410 core

in vec3 vNormal;

uniform vec3 uLightDir;
uniform vec3 uBaseColor;

out vec4 FragColor;

void main() {
    vec3  n        = normalize(vNormal);
    float diffuse  = max(dot(n, normalize(uLightDir)), 0.0);
    vec3  color    = uBaseColor * (0.2 + 0.8 * diffuse);
    FragColor      = vec4(color, 1.0);
}