#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <unordered_map>

#include "math/Mat4.h"
#include "math/Vec3.h"

class Shader {
    private:
        GLuint                                 _id = 0;
        std::unordered_map<std::string, GLint> _uniformLocations;
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        Shader(const Shader&)            = delete;
        Shader& operator=(const Shader&) = delete;

        void bind()   const;
        void unbind() const;

        void setBool (const std::string& name, bool value);
        void setInt  (const std::string& name, int value);
        void setFloat(const std::string& name, float value);
        void setVec3 (const std::string& name, const Vec3& value);
        void setMat4 (const std::string& name, const Mat4& value);

        GLuint id() const;
    private:
        GLint uniformLocation(const std::string& name);
};

#endif