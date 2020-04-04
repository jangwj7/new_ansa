#include <GLFW/glfw3.h>
#include <iostream>
#include "../glm/glm.hpp"
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

public:
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

