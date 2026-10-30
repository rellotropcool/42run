#include "gfx/Buffer.h"

Buffer::Buffer(GLenum target) : _target(target) {
    glGenBuffers(1, &_id);
}

Buffer::~Buffer() {
    if (_id) 
        glDeleteBuffers(1, &_id);
}

void Buffer::setData(const void* data, std::size_t bytes, GLenum usage) {
    bind();
    glBufferData(_target, static_cast<GLsizeiptr>(bytes), data, usage);
    _size = static_cast<GLsizeiptr>(bytes);
    unbind();
}

void Buffer::bind() const {
    glBindBuffer(_target, _id);
}

void Buffer::unbind() const {
    glBindBuffer(_target, 0);
}

GLuint Buffer::id() const { return _id; }
GLenum Buffer::target() const { return _target; }
GLsizeiptr Buffer::size() const { return _size; }