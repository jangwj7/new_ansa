#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>
#include <string>
// #include "key_input.h"
class Window
{
private:
    int height, width;
    // Key_input* key_input;
public:
    GLFWwindow* window;
    Window(int width, int height,std::string title);
    ~Window();
};
#endif