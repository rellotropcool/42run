#include "gfx/Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Shader: cannot open " + path);
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint compileStage(GLenum type, const std::string& source, const std::string& path) {
    GLuint stage = glCreateShader(type);
    const char* sourcePtr = source.c_str();
    glShaderSource(stage, 1, &sourcePtr, nullptr);
    glCompileShader(stage);

    GLint success = 0;
    glGetShaderiv(stage, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar log[1024];
        glGetShaderInfoLog(stage, sizeof(log), nullptr, log);
        glDeleteShader(stage);
        throw std::runtime_error("Shader compile error (" + path + "):\n" + log);
    }
    return stage;
}

}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource   = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    GLuint vertexStage   = compileStage(GL_VERTEX_SHADER,   vertexSource,   vertexPath);
    GLuint fragmentStage = compileStage(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    _id = glCreateProgram();
    glAttachShader(_id, vertexStage);
    glAttachShader(_id, fragmentStage);
    glLinkProgram(_id);

    GLint success = 0;
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar log[1024];
        glGetProgramInfoLog(_id, sizeof(log), nullptr, log);
        glDeleteShader(vertexStage);
        glDeleteShader(fragmentStage);
        glDeleteProgram(_id);
        _id = 0;
        throw std::runtime_error(std::string("Shader link error:\n") + log);
    }

    glDeleteShader(vertexStage);
    glDeleteShader(fragmentStage);
}

Shader::~Shader() {
    if (_id) glDeleteProgram(_id);
}

void Shader::bind() const {
    glUseProgram(_id);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(uniformLocation(name), value ? 1 : 0);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(uniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(uniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const Vec3& value) {
    glUniform3f(uniformLocation(name), value.x(), value.y(), value.z());
}

void Shader::setMat4(const std::string& name, const Mat4& value) {
    glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, value.data());
}

GLuint Shader::id() const { return _id; }

GLint Shader::uniformLocation(const std::string& name) {
    auto it = _uniformLocations.find(name);
    if (it != _uniformLocations.end())
        return it->second;

    GLint location = glGetUniformLocation(_id, name.c_str());
    _uniformLocations.emplace(name, location);
    return location;
}