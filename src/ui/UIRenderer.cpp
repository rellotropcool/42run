#include "ui/UIRenderer.h"

#include <glad/glad.h>

#include <algorithm>
#include <cstddef>

namespace {

const unsigned char _digitSegments[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
};

}

UIRenderer::UIRenderer()
    : _shader("shaders/ui.vert", "shaders/ui.frag")
    , _vbo(GL_ARRAY_BUFFER)
{
    VertexLayout layout;
    layout.stride(sizeof(Vertex));
    layout.addAttribute(VertexAttribute(0, 2, GL_FLOAT, GL_FALSE, 0));
    layout.addAttribute(VertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float)));
    _vao.addVertexBuffer(_vbo, layout);
}

void UIRenderer::beginFrame(float screenW, float screenH) {
    _screenW = screenW;
    _screenH = screenH;
    _vertices.clear();
}

void UIRenderer::drawNumber(int value, float x, float y,
                             float digitW, float digitH, const Vec3& color) {
    if (value < 0)
        value = 0;

    int digits[10];
    int n = 0;

    if (value == 0) {
        digits[n++] = 0;
    } else {
        while (value > 0 && n < 10) {
            digits[n++] = value % 10;
            value /= 10;
        }
    }

    float gap = digitW * 0.25f;
    for (int i = n - 1; i >= 0; --i) {
        drawDigit(digits[i], x, y, digitW, digitH, color);
        x += digitW + gap;
    }
}

void UIRenderer::drawDigit(int digit, float x, float y,
                            float w, float h, const Vec3& color) {
    if (digit < 0 || digit > 9)
        return;

    unsigned char mask = _digitSegments[digit];
    float t     = std::min(w, h) * 0.18f;
    float halfH = h * 0.5f;
    float halfT = t * 0.5f;

    if (mask & 0x01) drawQuad(x + halfT, y, w - t, t, color);
    if (mask & 0x02) drawQuad(x + w - t, y + halfT, t, halfH - t, color);
    if (mask & 0x04) drawQuad(x + w - t, y + halfH, t, halfH - t, color);
    if (mask & 0x08) drawQuad(x + halfT, y + h - t, w - t, t, color);
    if (mask & 0x10) drawQuad(x, y + halfH, t, halfH - t, color);
    if (mask & 0x20) drawQuad(x, y + halfT, t, halfH - t, color);
    if (mask & 0x40) drawQuad(x + halfT, y + halfH - halfT, w - t, t, color);
}

void UIRenderer::drawQuad(float x, float y, float w, float h, const Vec3& color) {
    float x0 = ndcX(x);
    float x1 = ndcX(x + w);
    float y0 = ndcY(y);
    float y1 = ndcY(y + h);

    Vertex v0;
    v0._x = x0; v0._y = y0; v0._r = color.x(); v0._g = color.y(); v0._b = color.z();

    Vertex v1;
    v1._x = x1; v1._y = y0; v1._r = color.x(); v1._g = color.y(); v1._b = color.z();

    Vertex v2;
    v2._x = x1; v2._y = y1; v2._r = color.x(); v2._g = color.y(); v2._b = color.z();

    Vertex v3;
    v3._x = x0; v3._y = y1; v3._r = color.x(); v3._g = color.y(); v3._b = color.z();

    _vertices.push_back(v0);
    _vertices.push_back(v1);
    _vertices.push_back(v2);
    _vertices.push_back(v0);
    _vertices.push_back(v2);
    _vertices.push_back(v3);
}

float UIRenderer::ndcX(float px) const {
    return px / _screenW * 2.0f - 1.0f;
}

float UIRenderer::ndcY(float py) const {
    return 1.0f - py / _screenH * 2.0f;
}

void UIRenderer::endFrame() {
    if (_vertices.empty())
        return;

    _vbo.setData(_vertices.data(), _vertices.size() * sizeof(Vertex),
                 GL_DYNAMIC_DRAW);

    glDisable(GL_DEPTH_TEST);

    _shader.bind();
    _vao.bind();
    _vao.drawArrays(static_cast<GLsizei>(_vertices.size()));
    _vao.unbind();

    glEnable(GL_DEPTH_TEST);
}