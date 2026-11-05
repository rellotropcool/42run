#version 410 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aColor;

out vec2 vUV;
out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vUV         = aUV;
    vColor      = aColor;
}