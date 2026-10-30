#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

class Window {
    private:
        GLFWwindow* _handle = nullptr;
        int         _width  = 0;
        int         _height = 0;
    public:
        Window(int width, int height, const char* title);
        ~Window();

        Window(const Window&)            = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose() const;
        void pollEvents()  const;
        void swapBuffers() const;
        void clear()       const;

        GLFWwindow* handle() const;
        int         width()  const;
        int         height() const;
};

#endif