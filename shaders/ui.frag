#version 410 core

in vec2 vUV;
in vec3 vColor;

uniform sampler2D uTexture;
uniform int       uMode;

uniform float     uFlicker;

uniform float     uPanelAlpha;
uniform float     uScreenH;
uniform vec2      uPanelMin;
uniform vec2      uPanelMax;
uniform float     uPanelCornerCut;
uniform float     uPanelBorderWidth;
uniform vec3      uPanelBorderColor;

out vec4 FragColor;

void main() {
    vec4 t = texture(uTexture, vUV);

    if (uMode == 1) {
        FragColor = vec4(vColor * t.rgb, t.a);
        return;
    }

    if (uMode == 2) {
        float px = gl_FragCoord.x;
        float py = uScreenH - gl_FragCoord.y;

        float x0 = uPanelMin.x;
        float y0 = uPanelMin.y;
        float x1 = uPanelMax.x;
        float y1 = uPanelMax.y;
        float c  = uPanelCornerCut;

        float dL  = px - x0;
        float dR  = x1 - px;
        float dT  = py - y0;
        float dB  = y1 - py;
        float dTR = (py - y0 - px + x1 - c) / 1.41421356;
        float dBL = ((y1 - py) - (x0 - px + c)) / 1.41421356;

        if (dL < 0.0 || dR < 0.0 || dT < 0.0 || dB < 0.0 ||
            dTR < 0.0 || dBL < 0.0) {
            discard;
        }

        float dMin = min(min(min(dL, dR), min(dT, dB)), min(dTR, dBL));

        if (dMin < uPanelBorderWidth) {
            FragColor = vec4(uPanelBorderColor * uFlicker, 1.0);
            return;
        }

        float scan = mod(gl_FragCoord.y, 8.0);
        vec3  col  = mix(vColor, vColor * 0.55, step(4.0, scan));
        FragColor  = vec4(col * uFlicker, uPanelAlpha * t.r);
        return;
    }

    FragColor = vec4(vColor * uFlicker, t.r);
}