#ifndef MESH_H
#define MESH_H

#include <cstddef>

#include "gfx/Buffer.h"
#include "gfx/VertexArray.h"

class Mesh {
    private:
        VertexArray _vao;
        Buffer      _vbo;
        Buffer      _ebo;
        GLsizei     _indexCount = 0;
    public:
        Mesh(const void*       vertexData,
             std::size_t       vertexBytes,
             const void*       indexData,
             std::size_t       indexCount,
             const VertexLayout& layout);
        ~Mesh();

        Mesh(const Mesh&)            = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&)                 = delete;
        Mesh& operator=(Mesh&&)      = delete;

        void draw() const;

        GLsizei indexCount() const;
};

#endif