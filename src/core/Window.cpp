#include "core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

Window::Window(int width, int height, const char* title)
    : _width(width), _height(height)
{
    if (!glfwInit())
        throw std::runtime_error("Window: glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    _handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!_handle) {
        glfwTerminate();
        throw std::runtime_error("Window: glfwCreateWindow failed");
    }

    glfwMakeContextCurrent(_handle);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(_handle);
        glfwTerminate();
        throw std::runtime_error("Window: gladLoadGLLoader failed");
    }

    glfwSwapInterval(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Window::~Window() {
    if (_handle) glfwDestroyWindow(_handle);
    glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(_handle); }
void Window::pollEvents()  const { glfwPollEvents(); }
void Window::swapBuffers() const { glfwSwapBuffers(_handle); }

void Window::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::handle() const { return _handle; }
int         Window::width()  const { return _width; }
int         Window::height() const { return _height; }