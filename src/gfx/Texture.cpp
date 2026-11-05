#include "gfx/Texture.h"

Texture::Texture() {
    glGenTextures(1, &_id);
}

Texture::~Texture() {
    if (_id) glDeleteTextures(1, &_id);
}

void Texture::setData(GLsizei width, GLsizei height,
                      GLenum internalFormat, GLenum format, GLenum type,
                      const void* data) {
    _width  = width;
    _height = height;

    bind(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat),
                 width, height, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unbind(0);
}

void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unbind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setFiltering(GLenum min, GLenum mag) {
    bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    unbind(0);
}

void Texture::setWrap(GLenum s, GLenum t) {
    bind(0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    unbind(0);
}

GLuint  Texture::id()     const { return _id; }
GLsizei Texture::width()  const { return _width; }
GLsizei Texture::height() const { return _height; }