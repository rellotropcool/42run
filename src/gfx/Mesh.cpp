#include "gfx/Mesh.h"

#include <cstdint>

Mesh::Mesh(const void*        vertexData,
           std::size_t        vertexBytes,
           const void*        indexData,
           std::size_t        indexCount,
           const VertexLayout& layout)
    : _vbo(GL_ARRAY_BUFFER)
    , _ebo(GL_ELEMENT_ARRAY_BUFFER)
    , _indexCount(static_cast<GLsizei>(indexCount))
{
    _vbo.setData(vertexData, vertexBytes, GL_STATIC_DRAW);
    _vao.addVertexBuffer(_vbo, layout);

    _ebo.setData(indexData, indexCount * sizeof(std::uint32_t), GL_STATIC_DRAW);
    _vao.setIndexBuffer(_ebo);
}

Mesh::~Mesh() = default;

void Mesh::draw() const {
    _vao.bind();
    _vao.drawIndexed(_indexCount);
}

GLsizei Mesh::indexCount() const { return _indexCount; }