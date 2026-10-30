#include "gfx/VertexArray.h"

VertexLayout makeVertexLayout() {
    VertexLayout layout;
    layout.stride(sizeof(Vertex));
    layout.addAttribute(VertexAttribute(0, 3, GL_FLOAT, GL_FALSE,
                                        offsetof(Vertex, position)));
    layout.addAttribute(VertexAttribute(1, 3, GL_FLOAT, GL_FALSE,
                                        offsetof(Vertex, normal)));
    layout.addAttribute(VertexAttribute(2, 2, GL_FLOAT, GL_FALSE,
                                        offsetof(Vertex, uv)));
    return layout;
}

// ---------------------------------------------------------------------------
// VertexAttribute
// ---------------------------------------------------------------------------
VertexAttribute::VertexAttribute(GLuint location, GLint size, GLenum type,
                                 GLboolean normalized, std::size_t offset)
    : _location(location), _size(size), _type(type),
      _normalized(normalized), _offset(offset) {}

GLuint      VertexAttribute::location()   const { return _location; }
GLint       VertexAttribute::size()       const { return _size; }
GLenum      VertexAttribute::type()       const { return _type; }
GLboolean   VertexAttribute::normalized() const { return _normalized; }
std::size_t VertexAttribute::offset()     const { return _offset; }

// ---------------------------------------------------------------------------
// VertexLayout
// ---------------------------------------------------------------------------
void VertexLayout::addAttribute(const VertexAttribute& attribute) {
    _attributes.push_back(attribute);
}

const std::vector<VertexAttribute>& VertexLayout::attributes() const {
    return _attributes;
}

GLsizei VertexLayout::stride() const { return _stride; }
void    VertexLayout::stride(GLsizei value) { _stride = value; }

// ---------------------------------------------------------------------------
// VertexArray
// ---------------------------------------------------------------------------
VertexArray::VertexArray() {
    glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray() {
    if (_id) glDeleteVertexArrays(1, &_id);
}

void VertexArray::addVertexBuffer(const Buffer& vbo, const VertexLayout& layout) {
    bind();
    vbo.bind();

    for (const auto& a : layout.attributes()) {
        glVertexAttribPointer(a.location(), a.size(), a.type(),
                              a.normalized(),
                              layout.stride(),
                              reinterpret_cast<const void*>(a.offset()));
        glEnableVertexAttribArray(a.location());
    }

    unbind();
}

void VertexArray::setIndexBuffer(const Buffer& ebo) {
    bind();
    ebo.bind();
    unbind();
}

void VertexArray::bind() const {
    glBindVertexArray(_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::drawIndexed(GLsizei indexCount) const {
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

GLuint VertexArray::id() const { return _id; }