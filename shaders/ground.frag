#version 410 core

in vec2 vUV;

uniform float uScrollOffset;
uniform vec3  uColor1;
uniform vec3  uColor2;

out vec4 FragColor;

void main() {
    vec2 tileCount = vec2(2.5, 125.0);
    vec2 scrolled  = vUV * tileCount + vec2(0.0, uScrollOffset);
    vec2 cell      = floor(scrolled);
    float checker  = mod(cell.x + cell.y, 2.0);

    FragColor = vec4(mix(uColor1, uColor2, checker), 1.0);
}