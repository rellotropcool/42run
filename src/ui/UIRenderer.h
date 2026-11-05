#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "gfx/Buffer.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "math/Vec3.h"

#include <vector>

class UIRenderer {
    private:
        struct Vertex {
            float _x = 0.0f;
            float _y = 0.0f;
            float _r = 1.0f;
            float _g = 1.0f;
            float _b = 1.0f;
        };

        Shader              _shader;
        Buffer              _vbo;
        VertexArray         _vao;
        std::vector<Vertex> _vertices;
        float               _screenW = 0.0f;
        float               _screenH = 0.0f;

    public:
        UIRenderer();
        ~UIRenderer() = default;

        UIRenderer(const UIRenderer&)            = delete;
        UIRenderer& operator=(const UIRenderer&) = delete;

        void beginFrame(float screenW, float screenH);
        void drawNumber(int value, float x, float y,
                        float digitW, float digitH, const Vec3& color);
        void endFrame();

    private:
        void  drawQuad (float x, float y, float w, float h, const Vec3& color);
        void  drawDigit(int digit, float x, float y, float w, float h, const Vec3& color);
        float ndcX(float px) const;
        float ndcY(float py) const;
};

#endif