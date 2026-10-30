#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>

#include <cstddef>
#include <vector>

#include "gfx/Buffer.h"
#include "math/Vec2.h"
#include "math/Vec3.h"

class VertexAttribute {
    private:
        GLuint      _location   = 0;
        GLint       _size       = 0;
        GLenum      _type       = GL_FLOAT;
        GLboolean   _normalized = GL_FALSE;
        std::size_t _offset     = 0;
    public:
        VertexAttribute() = default;
        VertexAttribute(GLuint location, GLint size, GLenum type,
                        GLboolean normalized, std::size_t offset);

        GLuint      location()   const;
        GLint       size()       const;
        GLenum      type()       const;
        GLboolean   normalized() const;
        std::size_t offset()     const;
};

class VertexLayout {
    private:
        std::vector<VertexAttribute> _attributes;
        GLsizei                      _stride = 0;
    public:
        VertexLayout() = default;

        void addAttribute(const VertexAttribute& attribute);

        const std::vector<VertexAttribute>& attributes() const;
        GLsizei                             stride()     const;
        void                                stride(GLsizei value);
};

class VertexArray {
    private:
        GLuint _id = 0;
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&)            = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void addVertexBuffer(const Buffer& vbo, const VertexLayout& layout);
        void setIndexBuffer(const Buffer& ebo);

        void bind()   const;
        void unbind() const;

        void drawIndexed(GLsizei indexCount) const;

        GLuint id() const;
};

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
};

VertexLayout makeVertexLayout();

#endif