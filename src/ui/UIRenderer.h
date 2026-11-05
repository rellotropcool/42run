#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "gfx/Buffer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
#include "math/Vec2.h"
#include "math/Vec3.h"

#include <string>
#include <vector>

class Font;

class UIRenderer {
    private:
        struct Vertex {
            float _x = 0.0f;
            float _y = 0.0f;
            float _u = 0.0f;
            float _v = 0.0f;
            float _r = 1.0f;
            float _g = 1.0f;
            float _b = 1.0f;
        };

        const Font& _bodyFont;
        const Font& _titleFont;
        Shader      _shader;
        Texture     _white;

        Buffer              _bgVbo;
        VertexArray         _bgVao;
        std::vector<Vertex> _bgVertices;
        const Texture*      _bgTexture = nullptr;

        Buffer              _panelVbo;
        VertexArray         _panelVao;
        std::vector<Vertex> _panelVertices;
        Vec2                _panelMin;
        Vec2                _panelMax;
        float               _panelAlpha       = 0.72f;
        float               _panelCornerCut   = 20.0f;
        float               _panelBorderWidth = 2.0f;
        Vec3                _panelBorderColor;

        Buffer              _rectVbo;
        VertexArray         _rectVao;
        std::vector<Vertex> _rectVertices;

        Buffer              _textVbo;
        VertexArray         _textVao;
        std::vector<Vertex> _textVertices;

        Buffer              _titleVbo;
        VertexArray         _titleVao;
        std::vector<Vertex> _titleVertices;

        float _screenW     = 0.0f;
        float _screenH     = 0.0f;
        float _flicker     = 1.0f;
        bool  _hintVisible = true;

    public:
        UIRenderer(const Font& bodyFont, const Font& titleFont);
        ~UIRenderer() = default;

        UIRenderer(const UIRenderer&)            = delete;
        UIRenderer& operator=(const UIRenderer&) = delete;

        void beginFrame(float screenW, float screenH, float time);
        void endFrame();

        void drawBackground(const Texture& texture);

        void drawPanel(float x, float y, float w, float h,
                       const Vec3& fill, float alpha,
                       const Vec3& border, float borderWidth,
                       float cornerCut);

        void drawRect(float x, float y, float w, float h, const Vec3& color);

        void drawText         (const std::string& text, float x, float y, const Vec3& color);
        void drawTextCentered (const std::string& text, float centerX, float y, const Vec3& color);
        void drawTitle        (const std::string& text, float x, float y, const Vec3& color);
        void drawTitleCentered(const std::string& text, float centerX, float y, const Vec3& color);

        float measureText (const std::string& text) const;
        float measureTitle(const std::string& text) const;
        bool  hintVisible() const;

    private:
        float ndcX(float px) const;
        float ndcY(float py) const;

        void batchGlyphs(const std::string& text, float x, float y,
                         const Vec3& color, const Font& font,
                         std::vector<Vertex>& out);

        static void drawBatch(Buffer& vbo, VertexArray& vao,
                              const std::vector<Vertex>& verts,
                              const Texture& texture);
};

#endif