#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "gfx/Buffer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/VertexArray.h"
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

        const Font&         _font;
        Shader              _shader;
        Texture             _white;

        Buffer              _rectVbo;
        VertexArray         _rectVao;
        std::vector<Vertex> _rectVertices;

        Buffer              _textVbo;
        VertexArray         _textVao;
        std::vector<Vertex> _textVertices;

        float               _screenW = 0.0f;
        float               _screenH = 0.0f;

    public:
        explicit UIRenderer(const Font& font);
        ~UIRenderer() = default;

        UIRenderer(const UIRenderer&)            = delete;
        UIRenderer& operator=(const UIRenderer&) = delete;

        void beginFrame(float screenW, float screenH);
        void endFrame();

        void drawRect(float x, float y, float w, float h, const Vec3& color);

        void drawText(const std::string& text, float x, float y,
                      const Vec3& color);
        void drawTextCentered(const std::string& text, float centerX, float y,
                              const Vec3& color);

        float measureText(const std::string& text) const;

    private:
        float ndcX(float px) const;
        float ndcY(float py) const;
};

#endif