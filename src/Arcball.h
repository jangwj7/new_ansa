#ifndef ARCBALL_h
#define ARCBALL_h

#include <iostream>

#include <GLFW/glfw3.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include "../glm/gtc/matrix_inverse.hpp"

class Arcball {
private:
    int windowWidth;
    int windowHeight;
    int mouseEvent;
    GLfloat rollSpeed;
    GLfloat angle;
    glm::vec3 prevPos;
    glm::vec3 currPos;
    
    
public:
    bool xAxis;
    bool yAxis;
    glm::vec3 camAxis;
    void changed_window_size(int width, int height);
    Arcball( int window_width, int window_height, GLfloat roll_speed = 1.0f, bool x_axis = true, bool y_axis = true );
    glm::vec3 toScreenCoord( double x, double y );

    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorCallback( GLFWwindow *window, double x, double y );
    
    glm::mat4 createViewRotationMatrix();
    glm::mat4 createModelRotationMatrix( glm::mat4& view_matrix );
    
};

#endif 
