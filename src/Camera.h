#ifndef CAMERA_H
#define CAMERA_H
#include <GLFW/glfw3.h>
#include <iostream>
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/rotate_vector.hpp"
#include "../glm/gtc/matrix_inverse.hpp"
class Camera
{
private:
    // from arcball
    int mouseEvent;
    float rollSpeed;
    float angle;
    glm::vec3 prevPos;
    glm::vec3 currPos;
    glm::vec3 position;
    // New arcball implementation
    // Calculates our position on the ArcBall from 2D mouse position.
    // \param tscMouse   TSC coordinates of mouse click.
    glm::vec3 mouseOnSphere(const glm::vec3& tscMouse);
    static glm::quat quatFromUnitSphere(const glm::vec3& from, const glm::vec3& to);
    glm::vec3     mCenter;        ///< Center of the arcball in target coordinate system.
    float  mRadius;        ///< Radius of the arcball in target coordinate system.

    glm::quat     mQNow;          ///< Current state of the rotation taking into account mouse.
                                  ///< Essentially QDrag * QDown (QDown is a applied first, just
                                  ///< as in matrix multiplication).
    glm::quat     mQDown;         ///< State of the rotation since mouse down.
    glm::quat     mQDrag;         ///< Dragged transform. Knows nothing of any prior 
                                  ///< transformations.

    glm::vec3     mVNow;          ///< Most current TCS position of mouse (during drag).
    glm::vec3     mVDown;         ///< TCS position of mouse when the drag was begun.
    glm::vec3     mVSphereFrom;   ///< vDown mapped to the sphere of 'mRadius' centered at 'mCenter' in TCS.
    glm::vec3     mVSphereTo;     ///< vNow mapped to the sphere of 'mRadius' centered at 'mCenter' in TCS.

    glm::mat4     mMatNow;        ///< Matrix representing the current rotation.

    /// Transform from screen coordinates to the target coordinate system.
    glm::mat4     mScreenToTCS;
public:
    glm::vec3 center = glm::vec3(0,0,0);
    float radius = 0.75;
    // Initiate an arc ball drag given the mouse click in screen coordinates.
    // mouseScreenCoords  Mouse screen coordinates.
    void beginDrag(const glm::vec2& mouseScreenCoords);

    //  Informs the arcball when the mouse has been dragged.
    //  mouseScreenCoords  Mouse screen coordinates.
    void drag(const glm::vec2& mouseScreenCoords);
    // Retrieves the current transformation in TCS.
    // Obtains full transformation of object in question. If the arc ball is 
    // being used to control camera rotation, then this will contain all
    // concatenated camera transformations. The current state of the camera
    // is stored in the quaternions mQDown and mQNow. mMatNow is calculated
    // from mQNow.
    glm::mat4 getTransformation() const;
    // end new arcball

    int windowWidth;
    int windowHeight;
    glm::vec3 front;
    glm::vec3 up; 
    glm::mat4 view;
    void set_viewport(int x,int y,int width, int height);
    void set_position(float x,float y,float z);
    glm::vec3 get_position(){return this->position;};
    Camera( glm::vec3 position,
            glm::vec3 front,
            glm::vec3 up,
            int window_width,
            int window_height, 
            GLfloat roll_speed = 1.0f,
            bool x_axis = true,
            bool y_axis = true );
    glm::mat4 get_lookAt();
    void update_view(glm::vec3 vector);
    void zoom(float x, float y,float z);
    ~Camera();
    // from arcball
    glm::vec3 camAxis;
    bool xAxis;
    bool yAxis;
    void changed_window_size(int width, int height);
    glm::vec3 toScreenCoord( double x, double y );
    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorCallback( GLFWwindow *window, double x, double y );
    glm::mat4 current_rotation_matrix;
    glm::mat4 previous_rotation_matrix;
    glm::mat4 createViewRotationMatrix();
    glm::mat4 createModelRotationMatrix( glm::mat4& view_matrix );
};
#endif
