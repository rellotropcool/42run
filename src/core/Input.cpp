#include "core/Input.h"

#include "core/Window.h"

#include <GLFW/glfw3.h>

Input::Input(const Window& window) : _window(window.handle()) {}

void Input::update() {
    _previous = _current;
    for (int k = 0; k < static_cast<int>(_current.size()); ++k)
        _current[k] = (glfwGetKey(_window, k) == GLFW_PRESS);
}

bool Input::isDown(int key) const {
    if (key < 0 || key >= static_cast<int>(_current.size()))
        return false;
    return _current[key];
}

bool Input::wasPressed(int key) const {
    if (key < 0 || key >= static_cast<int>(_current.size()))
        return false;
    return _current[key] && !_previous[key];
}

bool Input::wasReleased(int key) const {
    if (key < 0 || key >= static_cast<int>(_current.size()))
        return false;
    return !_current[key] && _previous[key];
}