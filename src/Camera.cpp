#include "Camera.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include "../glm/gtc/matrix_inverse.hpp"

// New arcball implementation


// end new arcball



void Camera::set_viewport(int x,int y,int width, int height){
    glViewport(x,y,width,height);
};

void Camera::set_position(float x,float y,float z){
    this->position = glm::vec3(x,y,z);
};
Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, // <-- camera related!
                int window_width, int window_height, // <-- window related
                float roll_speed, bool x_axis, bool y_axis ) // <-- arcball related 
{
    // camera related
    this->position = position;
    this->front = front;
    this->up = up;
    this->view = get_lookAt();
    // Window related
    this->windowWidth  = window_width;
    this->windowHeight = window_height;
    
    // new arcball

    // glm uses the following format for quaternions: w,x,y,z.
    //        w,    x,    y,    z
    glm::quat qOne(1.0, 0.0, 0.0, 0.0);
    glm::vec3 vZero(0.0, 0.0, 0.0);

    mVDown    = vZero;
    mVNow     = vZero;
    mQDown    = qOne;
    mQNow     = qOne;

    // old arcball 
    this->mouseEvent = 0;
    this->rollSpeed  = roll_speed;
    this->angle      = 0.0f;
    this->camAxis    = glm::vec3(0.0f, 0.0f, -1.0f);
    this->xAxis = x_axis;
    this->yAxis = y_axis;
};
glm::mat4 Camera::get_lookAt(){
    return glm::lookAt(position, position + front, up);
};
void Camera::zoom(float x, float y, float z){
    position += glm::vec3(x, y, z);
    this->view = glm::lookAt(position, front + position, up);
};
Camera::~Camera(){};


void Camera::changed_window_size(int width, int height){
    this->windowWidth   =  width;
    this->windowHeight  = height;
};

/**
 * Convert the mouse cursor coordinate on the window (i.e. from (0,0) to (windowWidth, windowHeight))
 * into normalized screen coordinate (i.e. (-1, -1) to (1, 1)
 */
glm::vec3 Camera::toScreenCoord( double x, double y ) {
    glm::vec3 coord(0.0f);
    
    if( xAxis )
        coord.x =  (2 * x - windowWidth ) / windowWidth;
    
    if( yAxis )
        coord.y = -(2 * y - windowHeight) / windowHeight;
    
    /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
    coord.x = glm::clamp( coord.x, -1.0f, 1.0f );
    coord.y = glm::clamp( coord.y, -1.0f, 1.0f );
    
    float length_squared = coord.x * coord.x + coord.y * coord.y;
    if( length_squared <= 1.0 )
        coord.z = sqrt( 1.0 - length_squared );
    else
        coord = glm::normalize( coord );
    
    return coord;
}

/**
 * Check whether we should start the mouse event
 * Event 0: when no tracking occured
 * Event 1: at the start of tracking, recording the first cursor pos
 * Event 2: tracking of subsequent cursor movement
 */
    void Camera::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    mouseEvent = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
}

void Camera::cursorCallback( GLFWwindow *window, double x, double y ){
    if( mouseEvent == 0 )
        return;
    else if( mouseEvent == 1 ) {
        /* Start of trackball, remember the first position */
        prevPos     = toScreenCoord( x, y );
        mouseEvent  = 2;
        return;
    }
    
    /* Tracking the subsequent */
    currPos  = toScreenCoord( x, y );
    
    /* Calculate the angle in radians, and clamp it between 0 and 90 degrees */
    angle    = acos( std::min(1.0f, glm::dot(prevPos, currPos) ));
    
    /* Cross product to get the rotation axis, but it's still in camera coordinate */
    camAxis  = glm::cross( prevPos, currPos );
}

/**
 * Create rotation matrix within the camera coordinate,
 * multiply this matrix with view matrix to rotate the camera
 */
glm::mat4 Camera::createViewRotationMatrix() {
    
    return glm::rotate( glm::degrees(angle) * rollSpeed, camAxis );
}

/**
 * Create rotation matrix within the world coordinate,
 * multiply this matrix with model matrix to rotate the object
 */
glm::mat4 Camera::createModelRotationMatrix( glm::mat4& view_matrix ){
    
    glm::vec3 axis = glm::inverse(glm::mat3(view_matrix)) * camAxis;
    return glm::rotate( glm::degrees(angle) * rollSpeed, axis );
}