#ifndef KEY_INPUT_H
#define KEY_INPUT_H
#include <GLFW/glfw3.h>
#include <iostream>

class Key_input
{
public:
    Key_input();
    ~Key_input();
    static void mouse_callback(GLFWwindow* window, int xpos, int ypos);
    static void window_size_callback(GLFWwindow *window, int width, int height);
    static void scroll_callback(GLFWwindow *window, double sideways, double z);
    static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow *window, double x, double y);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void process_input(GLFWwindow* window);
};
#endif