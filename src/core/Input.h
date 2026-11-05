#ifndef INPUT_H
#define INPUT_H

#include <array>

class Window;

class Input {
    private:
        struct GLFWwindow*    _window = nullptr;
        std::array<bool, 512> _current{};
        std::array<bool, 512> _previous{};
    public:
        explicit Input(const Window& window);

        void update();

        bool isDown(int key)      const;
        bool wasPressed(int key)  const;
        bool wasReleased(int key) const;
};

#endif