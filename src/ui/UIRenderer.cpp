#include "ui/UIRenderer.h"

#include "gfx/Font.h"

#include <glad/glad.h>

#include <algorithm>
#include <cstddef>

UIRenderer::UIRenderer(const Font& bodyFont, const Font& titleFont)
    : _bodyFont(bodyFont)
    , _titleFont(titleFont)
    , _shader("shaders/ui.vert", "shaders/ui.frag")
    , _bgVbo(GL_ARRAY_BUFFER)
    , _panelVbo(GL_ARRAY_BUFFER)
    , _rectVbo(GL_ARRAY_BUFFER)
    , _textVbo(GL_ARRAY_BUFFER)
    , _titleVbo(GL_ARRAY_BUFFER)
{
    unsigned char white[1] = { 255 };
    _white.setData(1, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, white);

    VertexLayout layout;
    layout.stride(sizeof(Vertex));
    layout.addAttribute(VertexAttribute(0, 2, GL_FLOAT, GL_FALSE, 0));
    layout.addAttribute(VertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float)));
    layout.addAttribute(VertexAttribute(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float)));

    _bgVao.addVertexBuffer(_bgVbo, layout);
    _panelVao.addVertexBuffer(_panelVbo, layout);
    _rectVao.addVertexBuffer(_rectVbo, layout);
    _textVao.addVertexBuffer(_textVbo, layout);
    _titleVao.addVertexBuffer(_titleVbo, layout);
}

void UIRenderer::beginFrame(float screenW, float screenH) {
    _screenW = screenW;
    _screenH = screenH;
    _bgVertices.clear();
    _bgTexture = nullptr;
    _panelVertices.clear();
    _rectVertices.clear();
    _textVertices.clear();
    _titleVertices.clear();
}

void UIRenderer::setFlicker(float flicker) {
    _flicker = flicker;
}

void UIRenderer::setHintVisible(bool visible) {
    _hintVisible = visible;
}

void UIRenderer::drawBackground(const Texture& texture) {
    if (texture.width() <= 0 || texture.height() <= 0)
        return;

    _bgTexture = &texture;

    float screenAspect = _screenW / _screenH;
    float imageAspect  = static_cast<float>(texture.width()) /
                         static_cast<float>(texture.height());

    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;

    if (screenAspect > imageAspect) {
        float visible = imageAspect / screenAspect;
        v0 = (1.0f - visible) * 0.5f;
        v1 = 1.0f - v0;
    } else if (screenAspect < imageAspect) {
        float visible = screenAspect / imageAspect;
        u0 = (1.0f - visible) * 0.5f;
        u1 = 1.0f - u0;
    }

    float ndcL = ndcX(0.0f);
    float ndcR = ndcX(_screenW);
    float ndcT = ndcY(0.0f);
    float ndcB = ndcY(_screenH);

    Vertex vtx0; vtx0._x = ndcL; vtx0._y = ndcT; vtx0._u = u0; vtx0._v = v0;
    Vertex vtx1; vtx1._x = ndcR; vtx1._y = ndcT; vtx1._u = u1; vtx1._v = v0;
    Vertex vtx2; vtx2._x = ndcR; vtx2._y = ndcB; vtx2._u = u1; vtx2._v = v1;
    Vertex vtx3; vtx3._x = ndcL; vtx3._y = ndcB; vtx3._u = u0; vtx3._v = v1;

    _bgVertices.push_back(vtx0);
    _bgVertices.push_back(vtx1);
    _bgVertices.push_back(vtx2);
    _bgVertices.push_back(vtx0);
    _bgVertices.push_back(vtx2);
    _bgVertices.push_back(vtx3);
}

void UIRenderer::drawPanel(float x, float y, float w, float h,
                           const Vec3& fill, float alpha,
                           const Vec3& border, float borderWidth,
                           float cornerCut) {
    _panelMin         = Vec2(x, y);
    _panelMax         = Vec2(x + w, y + h);
    _panelAlpha       = alpha;
    _panelCornerCut   = std::min(cornerCut, std::min(w, h) * 0.5f);
    _panelBorderWidth = borderWidth;
    _panelBorderColor = border;

    float x0 = ndcX(x);
    float x1 = ndcX(x + w);
    float y0 = ndcY(y);
    float y1 = ndcY(y + h);

    Vertex v0; v0._x = x0; v0._y = y0; v0._u = 0.0f; v0._v = 0.0f; v0._r = fill.x(); v0._g = fill.y(); v0._b = fill.z();
    Vertex v1; v1._x = x1; v1._y = y0; v1._u = 1.0f; v1._v = 0.0f; v1._r = fill.x(); v1._g = fill.y(); v1._b = fill.z();
    Vertex v2; v2._x = x1; v2._y = y1; v2._u = 1.0f; v2._v = 1.0f; v2._r = fill.x(); v2._g = fill.y(); v2._b = fill.z();
    Vertex v3; v3._x = x0; v3._y = y1; v3._u = 0.0f; v3._v = 1.0f; v3._r = fill.x(); v3._g = fill.y(); v3._b = fill.z();

    _panelVertices.push_back(v0);
    _panelVertices.push_back(v1);
    _panelVertices.push_back(v2);
    _panelVertices.push_back(v0);
    _panelVertices.push_back(v2);
    _panelVertices.push_back(v3);
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

void UIRenderer::batchGlyphs(const std::string& text, float x, float y,
                             const Vec3& color, const Font& font,
                             std::vector<Vertex>& out, float scale) {
    float penX      = x;
    float baselineY = y + font.ascent() * scale;

    for (char c : text) {
        const Font::Glyph& g = font.glyph(c);

        if (g.xoff2 <= g.xoff || g.yoff2 <= g.yoff) {
            penX += g.advance * scale;
            continue;
        }

        float gl_ = penX + g.xoff  * scale;
        float gr_ = penX + g.xoff2 * scale;
        float gt_ = baselineY + g.yoff  * scale;
        float gb_ = baselineY + g.yoff2 * scale;

        float ndcL = ndcX(gl_);
        float ndcR = ndcX(gr_);
        float ndcT = ndcY(gt_);
        float ndcB = ndcY(gb_);

        Vertex v0; v0._x = ndcL; v0._y = ndcT; v0._u = g.u0; v0._v = g.v0; v0._r = color.x(); v0._g = color.y(); v0._b = color.z();
        Vertex v1; v1._x = ndcR; v1._y = ndcT; v1._u = g.u1; v1._v = g.v0; v1._r = color.x(); v1._g = color.y(); v1._b = color.z();
        Vertex v2; v2._x = ndcR; v2._y = ndcB; v2._u = g.u1; v2._v = g.v1; v2._r = color.x(); v2._g = color.y(); v2._b = color.z();
        Vertex v3; v3._x = ndcL; v3._y = ndcB; v3._u = g.u0; v3._v = g.v1; v3._r = color.x(); v3._g = color.y(); v3._b = color.z();

        out.push_back(v0);
        out.push_back(v1);
        out.push_back(v2);
        out.push_back(v0);
        out.push_back(v2);
        out.push_back(v3);

        penX += g.advance * scale;
    }
}

void UIRenderer::drawText(const std::string& text, float x, float y,
                          const Vec3& color) {
    batchGlyphs(text, x, y, color, _bodyFont, _textVertices);
}

void UIRenderer::drawTextCentered(const std::string& text, float centerX, float y,
                                  const Vec3& color) {
    float w = _bodyFont.measureText(text);
    drawText(text, centerX - w * 0.5f, y, color);
}

void UIRenderer::drawTitle(const std::string& text, float x, float y,
                           const Vec3& color) {
    batchGlyphs(text, x, y, color, _titleFont, _titleVertices);
}

void UIRenderer::drawTitleCentered(const std::string& text, float centerX, float y,
                                   const Vec3& color) {
    float w = _titleFont.measureText(text);
    drawTitle(text, centerX - w * 0.5f, y, color);
}

void UIRenderer::drawTitleCenteredScaled(const std::string& text,
                                         float centerX, float centerY,
                                         const Vec3& color, float scale) {
    float fontH = (_titleFont.ascent() + _titleFont.descent()) * scale;
    float topY  = centerY - fontH * 0.5f;

    float baseWidth   = _titleFont.measureText(text);
    float scaledWidth = baseWidth * scale;
    float startX      = centerX - scaledWidth * 0.5f;

    batchGlyphs(text, startX, topY, color, _titleFont, _titleVertices, scale);
}

void UIRenderer::drawHint(const std::string& text, float centerX, float y,
                          const Vec3& color) {
    if (!_hintVisible)
        return;
    drawTextCentered(text, centerX, y, color);
}

float UIRenderer::measureText(const std::string& text) const {
    return _bodyFont.measureText(text);
}

float UIRenderer::measureTitle(const std::string& text) const {
    return _titleFont.measureText(text);
}

float UIRenderer::measureTitleHeight() const {
    return _titleFont.ascent() + _titleFont.descent();
}

void UIRenderer::drawBatch(Buffer& vbo, VertexArray& vao,
                           const std::vector<Vertex>& verts,
                           const Texture& texture) {
    if (verts.empty())
        return;

    vbo.setData(verts.data(), verts.size() * sizeof(Vertex), GL_DYNAMIC_DRAW);
    texture.bind(0);
    vao.bind();
    vao.drawArrays(static_cast<GLsizei>(verts.size()));
    vao.unbind();
}

void UIRenderer::endFrame() {
    if (_bgVertices.empty() && _panelVertices.empty() &&
        _rectVertices.empty() && _textVertices.empty() && _titleVertices.empty())
        return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _shader.bind();
    _shader.setInt  ("uTexture", 0);
    _shader.setFloat("uFlicker", _flicker);

    if (!_bgVertices.empty() && _bgTexture) {
        _shader.setInt("uMode", 1);
        drawBatch(_bgVbo, _bgVao, _bgVertices, *_bgTexture);
    }

    if (!_panelVertices.empty()) {
        _shader.setInt  ("uMode",             2);
        _shader.setFloat("uScreenH",          _screenH);
        _shader.setVec2 ("uPanelMin",         _panelMin);
        _shader.setVec2 ("uPanelMax",         _panelMax);
        _shader.setFloat("uPanelAlpha",       _panelAlpha);
        _shader.setFloat("uPanelCornerCut",   _panelCornerCut);
        _shader.setFloat("uPanelBorderWidth", _panelBorderWidth);
        _shader.setVec3 ("uPanelBorderColor", _panelBorderColor);
        drawBatch(_panelVbo, _panelVao, _panelVertices, _white);
    }

    _shader.setInt("uMode", 0);

    drawBatch(_rectVbo,  _rectVao,  _rectVertices,  _white);
    drawBatch(_textVbo,  _textVao,  _textVertices,  _bodyFont.atlas());
    drawBatch(_titleVbo, _titleVao, _titleVertices, _titleFont.atlas());

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

float UIRenderer::ndcX(float px) const {
    return px / _screenW * 2.0f - 1.0f;
}

float UIRenderer::ndcY(float py) const {
    return 1.0f - py / _screenH * 2.0f;
}