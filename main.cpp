#include "src/imgui/imgui.h"
#include "src/imgui/imgui_impl_glfw.h"
#include "src/imgui/imgui_impl_opengl3.h"
#include "src/imgui/ImGuiFileBrowser.h"
#include "src/IconsFontAwesome5.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "src/read_abaqus.h"
#include "src/read_abaqus.h"
#include "src/shaders/shader.h"
#include "src/Camera.h"
#include "src/common.h"
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <fstream>

static bool pid_window(bool* p_open, Mesh& mesh);
static bool element_window(bool* p_open, Mesh& mesh);
static bool element_window(bool* p_open, Mesh& mesh);
static bool mid_window(bool* p_open, Mesh& mesh);
void callback_window_size(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow *window, double sideways, double z);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void cursorCallback(GLFWwindow *window, double x, double y);

const unsigned int SCR_WIDTH    = 800;
const unsigned int SCR_HEIGHT   = 600;



// Create camera
// pos, front, up
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f),
              glm::vec3(0.0f, 0.0f, -1.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              SCR_WIDTH,
              SCR_HEIGHT,
              0.15f,
              true,
              true);
Camera coord_camera(glm::vec3(0.0f, 0.0f, 300.0f),
              glm::vec3(0.0f, 0.0f, -1.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              SCR_WIDTH,
              SCR_HEIGHT,
              0.15f,
              true,
              true);

int main(int argc, char* argv[])
{
    Mesh mesh;
    // read input files
    if (argc > 1)
    {
        for (int i = 0; i < argc; i++)
        {
            mesh.read_file(std::string(argv[1]));
            // std::cout << "Reading file " << argv[1] << "\n";
        }
    }
    // Adjust camera according to CoG and max coords
    camera.set_position(0,0,0);
    
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
    GLFWwindow *window = glfwCreateWindow(800, 600, "viewer", NULL, NULL);
    if (window == NULL)
    {
        // window wasnt created
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Need to init glad before we call openGL
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed GLAD" << std::endl;
        return -1;
    }



    // set icon
    // GLFWimage icon[1];
    // icon[0].pixels = SOIL_load_image("src/resources/icon.png", &icon[0].width, &icon[0].height, 0, SOIL_LOAD_RGBA);
    // glfwSetWindowIcon(window, 1, icon);
    // release data
    // SOIL_free_image_data(icon[0].pixels);
    // load an image file directly as a new OpenGL texture 
    // ====================
    // Texture 1
    // ====================
    // Load, create texture and generate mipmaps
    GLuint icon_node_marker = common::generate_texture("src/resources/icon_node_marker.png",true);
    GLuint icon_show_nodes  = common::generate_texture("src/resources/icon_show_nodes.png",true);
    GLuint icon_show_mesh   = common::generate_texture("src/resources/icon_show_mesh.png",true);
    // Set callback to when window is resized to a function that
    // sets the viewport
    glfwSetFramebufferSizeCallback(window, callback_window_size);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // To enable depth & anti-aliasing on lines
    glEnable(GL_DEPTH_TEST);
    // To enable different point sizes
    glEnable(GL_PROGRAM_POINT_SIZE);
    // To enable sprites on points (node markers)  
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // anti-alias lines
    glEnable(GL_MULTISAMPLE);  

    // ---- SHADERS ----------------------------
    // background - gradient background behind mesh
    // base - mesh shader 
    // coord - coordinate system
    // node - 2D icon for each node
    Shader background_shader("src/shaders/background.vs","src/shaders/background.fs");
    Shader base_shader("src/shaders/light_and_wireframe.vs", "src/shaders/light_and_wireframe.fs", "src/shaders/light_and_wireframe.gs");
    Shader coord_shader("src/shaders/coord.vs","src/shaders/coord.fs");
    Shader node_shader("src/shaders/nodes.vs","src/shaders/nodes.fs");
    // ---- END SHADERS ------------------------
    unsigned int background_VAO;
    glGenVertexArrays(1, &background_VAO);
    glBindVertexArray(background_VAO);
    glBindVertexArray(0);

    unsigned int mesh_VBO, mesh_VAO;
    glGenVertexArrays(1, &mesh_VAO);
    glGenBuffers(1,      &mesh_VBO);
    glBindVertexArray(mesh_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.faces_vertices.size() * sizeof(mesh.faces_vertices.front()),
                 &mesh.faces_vertices.front(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(4 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(7 * sizeof(float)));
    // unbind VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // read coord system vertex data
    // coord system
    unsigned int coord_VBO,coord_VAO;
    std::vector<float> coord_system = common::read_vertex_file("src/resources/coord_system.txt");
    glGenVertexArrays(1, &coord_VAO);
    glGenBuffers(1,      &coord_VBO);
    glBindVertexArray(coord_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, coord_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 coord_system.size() * sizeof(coord_system.front()),
                 &coord_system.front(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(4 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void *)(7 * sizeof(float)));
    // unbind VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Change font
    io.Fonts->AddFontFromFileTTF("src/Roboto-Regular.ttf", 16);
    // create a file browser instance
    imgui_addons::ImGuiFileBrowser file_dialog;

    float near_depth_range  = 0.1;
    float far_depth_range   = 1000.0;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_depth_range, far_depth_range);
    // ortho callsign:left,right,bottom,top,zNear,zFar
    // float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    
    // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho( 0.f, (float)SCR_WIDTH, 0.f,(float)SCR_HEIGHT, near_depth_range, far_depth_range );
    bool hide_show_nodes=false, hide_show_mesh=true;
    // gradient color for background won't change each loop so set them now anyway..
    
    while (!glfwWindowShouldClose(window))
    {

        // 1. Events
        if (io.WantCaptureMouse == false or io.WantCaptureMouse == false)
        {
            processInput(window);
        }
        
        // Set viewport of comeplete screen
        camera.set_viewport(0,0,camera.windowWidth,camera.windowHeight);
        // 2. Render
        // Clear the screen and reset to specific color
        // glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        // Draw background gradient
        glDisable(GL_DEPTH_TEST);
        // base_shader.setVec2("resolution", glm::vec2(camera.windowWidth,camera.windowHeight));
        background_shader.use();
        background_shader.setVec2("resolution", camera.windowWidth,camera.windowHeight);
        background_shader.setVec3("top_color",   glm::vec3(0.2,0.278,0.34));
        background_shader.setVec3("bottom_color",glm::vec3(0,0.0784,0.141));
        glBindVertexArray(background_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glEnable(GL_DEPTH_TEST);
        // feed inputs to imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // activate mesh shader
        glm::mat4 rotated_model = model * camera.createModelRotationMatrix(camera.view);
        base_shader.use();
        base_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        base_shader.setVec3("lightPos", camera.get_position());
        base_shader.setVec3("viewPos",  camera.get_position());
        base_shader.setBool("hide_show_mesh",hide_show_mesh);
        base_shader.setMat4("ProjectionMatrix", projection);
        base_shader.setMat4("ViewMatrix", camera.view);
        base_shader.setMat4("ModelMatrix", rotated_model);
        glBindVertexArray(mesh_VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh.faces_vertices.size());
        // draw coord system int upper right corner
        if (hide_show_nodes == true)
        {
            node_shader.use();
            node_shader.setMat4("ProjectionMatrix", projection);
            node_shader.setMat4("ViewMatrix", camera.view);
            node_shader.setMat4("ModelMatrix", rotated_model);
            node_shader.setInt("tex", 0);
            glBindTexture(GL_TEXTURE_2D, icon_node_marker);
            glActiveTexture(GL_TEXTURE0);
            glDrawArrays(GL_POINTS, 0, mesh.faces_vertices.size());
        }
        
        // draw coord system
        coord_camera.set_viewport(camera.windowWidth-175,camera.windowHeight-200,175,175);
        coord_shader.use();
        glBindVertexArray(coord_VAO);
        coord_shader.setMat4("ProjectionMatrix", projection);
        coord_shader.setMat4("ViewMatrix", coord_camera.view);
        coord_shader.setMat4("ModelMatrix", rotated_model);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, coord_system.size());
        // render your GUI
        bool open = false, save = false;
        static bool pid_window_bool;
        static bool element_window_bool;
        ImGuiIO& io = ImGui::GetIO();
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Input file","ctrl + a"))
                {
                    // open file dialog when user clicks this button
                    open = true;
                }
                if (ImGui::MenuItem("Close","escape"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::Button("PID"))
            {
                pid_window_bool=true;
            }
            if (ImGui::Button("MID"))
            {
            }
            if (ImGui::Button("Elements"))
            {
                element_window_bool=true;
            }
            if (ImGui::Button("Nodes"))
            {
            }
            
            ImGui::EndMainMenuBar();
        }
        if (ImGui::BeginBottomMainMenuBar())
        {
            if (ImGui::ImageButton((void*)(intptr_t)icon_show_nodes,ImVec2(30,30)))
            {
                hide_show_nodes = !hide_show_nodes;
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((void*)(intptr_t)icon_show_mesh,ImVec2(30,30)))
            {
                hide_show_mesh = !hide_show_mesh;
            }
            ImGui::End();
        }
        pid_window(&pid_window_bool,mesh);
        element_window(&element_window_bool,mesh);
        
        // overlay 
        bool overlay = true;
        int corner = 0;
        const float DISTANCE = 10.0f;
        ImVec2 window_pos = ImVec2(DISTANCE, io.DisplaySize.y - 110);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.6f); // Transparent background
        if (ImGui::Begin("overlay", &overlay, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("view v0.1");
            ImGui::Separator();
            std::string text = mesh.get_filename();
            ImGui::Text(text.c_str());
        }
        ImGui::End();
           
        if(open){
            ImGui::OpenPopup("Open File");
        }
            
        if(file_dialog.showOpenFileDialog("Open File", ImVec2(700, 310), ".inp")){
            std::cout << mesh.faces_vertices.size() << "\n";
            mesh.read_file(file_dialog.selected_fn);
            // Need to update faces_vertices and upload to graphics card
            glBindVertexArray(mesh_VAO);
            glBufferData(GL_ARRAY_BUFFER,
                 mesh.faces_vertices.size() * sizeof(mesh.faces_vertices.front()),
                 &mesh.faces_vertices.front(),
                 GL_STATIC_DRAW);
            int counter = 1;
            std::cout << mesh.faces_vertices.size() << "\n";
            for (int i = 0; i < 100; i++)
            {
                std::cout << mesh.faces_vertices.at(i) << ",";
                counter++;
                if (counter == 11)
                {
                    counter = 1;
                std::cout << "\n";
                }
                
                        
            }
            
        }
        rotated_model = glm::mat4();
        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // 3. Update buffer
        glfwSwapBuffers(window);
        // Check if any event has occured, button or mouse etc
        glfwPollEvents();
    }    
    // int counter = 1;
    // std::ofstream myfile;
    // myfile.open ("example.txt");
    // myfile << "Writing this to a file.\n";
    // for (int i = 0; i < mesh.faces_vertices.size(); i++)
    // {
    //     myfile << mesh.faces_vertices.at(i) << ",";
    //     counter++;
    //     if (counter == 11)
    //     {
    //         counter = 1;
    //         myfile << "\n";
    //     }    
    // }
    // myfile.close();
    
    
    // clean up gui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
};


static bool element_window(bool* open,Mesh& mesh){
    if (*open == true && mesh.get_number_of_elements() > 0)
    {
        // 
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.4f, io.DisplaySize.y * 0.25f));
        ImGui::Begin("Element\'s",open,ImGuiWindowFlags_NoResize);
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        Element* temp_element_pointer;
        for (int i = 0; i < mesh.get_number_of_elements()-1; i++)
        {
            temp_element_pointer = mesh.get_element(i);
            if (ImGui::Selectable(std::to_string(temp_element_pointer->global_id).c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();
        // right    
        ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Separator(); 
            ImGui::Text("id: ");
            ImGui::SameLine();
            temp_element_pointer = mesh.get_element(selected);
            ImGui::Text(std::to_string(temp_element_pointer->global_id).c_str());
            for(auto const& pair: temp_element_pointer->options){
                std::string temp = pair.first + ": " + pair.second;
                ImGui::Text(temp.c_str());
            }
            
            ImGui::SameLine();
            ImGui::EndChild();
        ImGui::EndGroup();       
        // sista end?
        ImGui::End();
    } 
};

static bool pid_window(bool* open,Mesh& mesh){
    if (*open == true && mesh.get_pid_counter() > 0)
    {
        // 
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.4f, io.DisplaySize.y * 0.25f));
        ImGui::Begin("PID\'s",open,ImGuiWindowFlags_NoResize);
        ImGui::Text("aaa");
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < mesh.get_pid_counter()-1; i++)
        {
            Pid* temp_pid_pointer = mesh.get_pid(i);
            if (ImGui::Selectable(temp_pid_pointer->name.c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();
        // right    
        ImGui::BeginGroup();
            ImGui::Separator();            
            Pid* temp_pid_pointer = mesh.get_pid(selected);
            float color[3] = {temp_pid_pointer->r,temp_pid_pointer->g,temp_pid_pointer->b};
            ImVec4 color_vec4 = ImVec4(temp_pid_pointer->r,temp_pid_pointer->g,temp_pid_pointer->b,1.0f);
            if (ImGui::ColorButton("Click to select color", color_vec4))
            {                
            }
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            // TODO: write 
            ImGui::Text("MID = ");
            ImGui::SameLine();
            // ImGui::Text(temp_pid_pointer->mid
            ImGui::EndChild();
            if (ImGui::Button("Highlight selected")) {}
        ImGui::EndGroup();       
        // sista end?
        ImGui::End();
    } 
}

void callback_window_size(GLFWwindow *window, int width, int height)
{
    // Change the viewport according to window size
    glViewport(0, 0, width, height);
    // Update arcball
    camera.changed_window_size(width,height);
}
void scroll_callback(GLFWwindow *window, double sideways, double z){
    camera.zoom(0,0,(float)z);    
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.zoom(0.1, 0, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera.zoom(-0.1, 0, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.zoom(0, -0.1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera.zoom(0, 0.1, 0);
    }
}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    /* Pass the arguments to our arcball object */
        camera.mouseButtonCallback(window, button, action, mods);
}

void cursorCallback(GLFWwindow *window, double x, double y)
{
    /* Pass the arguments to our arcball object */
    camera.cursorCallback(window, x, y);
    // std::cout << x << "\n";
    // std::cout << y << "\n";
}
