#include "ui/UIRenderer.h"

#include "gfx/Font.h"

#include <glad/glad.h>

#include <cstddef>

UIRenderer::UIRenderer(const Font& font)
    : _font(font)
    , _shader("shaders/ui.vert", "shaders/ui.frag")
    , _rectVbo(GL_ARRAY_BUFFER)
    , _textVbo(GL_ARRAY_BUFFER)
{
    unsigned char white[1] = { 255 };
    _white.setData(1, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, white);

    VertexLayout layout;
    layout.stride(sizeof(Vertex));
    layout.addAttribute(VertexAttribute(0, 2, GL_FLOAT, GL_FALSE, 0));
    layout.addAttribute(VertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float)));
    layout.addAttribute(VertexAttribute(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float)));

    _rectVao.addVertexBuffer(_rectVbo, layout);
    _textVao.addVertexBuffer(_textVbo, layout);
}

void UIRenderer::beginFrame(float screenW, float screenH) {
    _screenW = screenW;
    _screenH = screenH;
    _rectVertices.clear();
    _textVertices.clear();
}

void UIRenderer::drawRect(float x, float y, float w, float h,
                          const Vec3& color) {
    float x0 = ndcX(x);
    float x1 = ndcX(x + w);
    float y0 = ndcY(y);
    float y1 = ndcY(y + h);

    Vertex v0; v0._x = x0; v0._y = y0; v0._r = color.x(); v0._g = color.y(); v0._b = color.z();
    Vertex v1; v1._x = x1; v1._y = y0; v1._r = color.x(); v1._g = color.y(); v1._b = color.z();
    Vertex v2; v2._x = x1; v2._y = y1; v2._r = color.x(); v2._g = color.y(); v2._b = color.z();
    Vertex v3; v3._x = x0; v3._y = y1; v3._r = color.x(); v3._g = color.y(); v3._b = color.z();

    _rectVertices.push_back(v0);
    _rectVertices.push_back(v1);
    _rectVertices.push_back(v2);
    _rectVertices.push_back(v0);
    _rectVertices.push_back(v2);
    _rectVertices.push_back(v3);
}

void UIRenderer::drawText(const std::string& text, float x, float y,
                          const Vec3& color) {
    float penX      = x;
    float baselineY = y + _font.ascent();

    for (char c : text) {
        const Font::Glyph& g = _font.glyph(c);

        if (g.xoff2 <= g.xoff || g.yoff2 <= g.yoff) {
            penX += g.advance;
            continue;
        }

        float gl_ = penX + g.xoff;
        float gr_ = penX + g.xoff2;
        float gt_ = baselineY + g.yoff;
        float gb_ = baselineY + g.yoff2;

        float ndcL = ndcX(gl_);
        float ndcR = ndcX(gr_);
        float ndcT = ndcY(gt_);
        float ndcB = ndcY(gb_);

        Vertex v0; v0._x = ndcL; v0._y = ndcT; v0._u = g.u0; v0._v = g.v0; v0._r = color.x(); v0._g = color.y(); v0._b = color.z();
        Vertex v1; v1._x = ndcR; v1._y = ndcT; v1._u = g.u1; v1._v = g.v0; v1._r = color.x(); v1._g = color.y(); v1._b = color.z();
        Vertex v2; v2._x = ndcR; v2._y = ndcB; v2._u = g.u1; v2._v = g.v1; v2._r = color.x(); v2._g = color.y(); v2._b = color.z();
        Vertex v3; v3._x = ndcL; v3._y = ndcB; v3._u = g.u0; v3._v = g.v1; v3._r = color.x(); v3._g = color.y(); v3._b = color.z();

        _textVertices.push_back(v0);
        _textVertices.push_back(v1);
        _textVertices.push_back(v2);
        _textVertices.push_back(v0);
        _textVertices.push_back(v2);
        _textVertices.push_back(v3);

        penX += g.advance;
    }
}

void UIRenderer::drawTextCentered(const std::string& text, float centerX, float y,
                                  const Vec3& color) {
    float w = _font.measureText(text);
    drawText(text, centerX - w * 0.5f, y, color);
}

float UIRenderer::measureText(const std::string& text) const {
    return _font.measureText(text);
}

void UIRenderer::endFrame() {
    if (_rectVertices.empty() && _textVertices.empty())
        return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _shader.bind();
    _shader.setInt("uTexture", 0);

    if (!_rectVertices.empty()) {
        _rectVbo.setData(_rectVertices.data(),
                         _rectVertices.size() * sizeof(Vertex),
                         GL_DYNAMIC_DRAW);
        _white.bind(0);
        _rectVao.bind();
        _rectVao.drawArrays(static_cast<GLsizei>(_rectVertices.size()));
        _rectVao.unbind();
    }

    if (!_textVertices.empty()) {
        _textVbo.setData(_textVertices.data(),
                         _textVertices.size() * sizeof(Vertex),
                         GL_DYNAMIC_DRAW);
        _font.atlas().bind(0);
        _textVao.bind();
        _textVao.drawArrays(static_cast<GLsizei>(_textVertices.size()));
        _textVao.unbind();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

float UIRenderer::ndcX(float px) const {
    return px / _screenW * 2.0f - 1.0f;
}

float UIRenderer::ndcY(float py) const {
    return 1.0f - py / _screenH * 2.0f;
}