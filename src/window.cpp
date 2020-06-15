#include <GLFW/glfw3.h>
#include "window.h"
#include <string>
#include <iostream>
Window::~Window(){};

Window::Window(int width, int height,std::string title){
    if (!glfwInit())
    {
        std::cout << "Initialization failed\n";
    }
    // Want to use openGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // TODO: use last parameters to make full screen?
    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        // window wasnt created
        std::cout << "Failed to create window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    // this->width = width;
    // this->height = height;
    // this->key_input = key;
    
    // glfwSetMouseButtonCallback(this->window,     key->mouse_callback);
    // glfwSetFramebufferSizeCallback(this->window, window_size_callback);
    // glfwSetScrollCallback(this->window,          scroll_callback);
    // glfwSetCursorPosCallback(this->window,       cursor_callback);
    // glfwSetKeyCallback(this->window,             key_callback);
};

// void Window::process_input(GLFWwindow* window){
    
//     std::cout << ":)";
// };

