#version 410 core

in vec2 vUV;
in vec3 vColor;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    float coverage = texture(uTexture, vUV).r;
    FragColor      = vec4(vColor, coverage);
}