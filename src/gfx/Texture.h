#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture {
    private:
        GLuint  _id     = 0;
        GLsizei _width  = 0;
        GLsizei _height = 0;
    public:
        Texture();
        ~Texture();

        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;

        void setData(GLsizei width, GLsizei height,
                     GLenum internalFormat, GLenum format, GLenum type,
                     const void* data);

        void bind(int unit)   const;
        void unbind(int unit) const;

        void setFiltering(GLenum min, GLenum mag);
        void setWrap(GLenum s, GLenum t);

        GLuint  id()     const;
        GLsizei width()  const;
        GLsizei height() const;
};

#endif