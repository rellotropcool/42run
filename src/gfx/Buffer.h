#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>
#include <cstddef>

class Buffer {
    private:
        GLuint     _id     = 0;
        GLenum     _target = 0;
        GLsizeiptr _size   = 0;
    public:
        Buffer(GLenum target);
        ~Buffer();

        Buffer(const Buffer&)            = delete;
        Buffer& operator=(const Buffer&) = delete;

        void setData(const void* data, std::size_t bytes,
                    GLenum usage = GL_STATIC_DRAW);

        void bind()   const;
        void unbind() const;

        GLuint     id()     const;
        GLenum     target() const;
        GLsizeiptr size()   const;
};

#endif