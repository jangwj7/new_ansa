// a Mesh object is the complete Mesh from an input file.
// a Mesh has nodes, elements, pid, mid.
#include "read_abaqus.h"
#include "misc_string_functions.h"
#include "element_geometries.h"
#include "../glm/gtx/normal.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>

// initalize some static counters...
unsigned int Mesh::node_counter     = 0;
unsigned int Mesh::element_counter  = 0;
unsigned int Mesh::pid_counter      = 0;
unsigned int Mesh::mid_counter      = 0;

// void Mesh::update_vertices(Element* element){
//     unsigned int local_element_id = element->local_id;
//     void glBufferSubData(GL_ARRAY_BUFFER, local_element_id*9, sizeof(float), const void *);
// };

// void Mesh::update_vertices(Pid* pid){
//     void glBufferSubData(GL_ARRAY_BUFFER, 9, sizeof(float), const void *data);
// };


void Mesh::add_mid(std::unordered_map<std::string, std::string> options){
    // Create new MID based on input on *MATERIAL data line.
    // Other functions add mtrl data such as density etc
    Mid temp_mid(mid_counter,options["NAME"]);
    mid_counter++;
    
};

void Mesh::calculate_cog(std::vector<Node>& in_nodes){
    float sum_x=0,sum_y=0,sum_z=0;
    for (int i = 0; i < in_nodes.size(); i++)
    {
        sum_x = in_nodes.at(i).x;
        sum_y = in_nodes.at(i).y;
        sum_z = in_nodes.at(i).z;
    }
    this->cog_x=sum_x/in_nodes.size();
    this->cog_y=sum_y/in_nodes.size();
    this->cog_z=sum_z/in_nodes.size();
};



Mesh::Mesh(){
    // init object to hold the mesh data.

    // read text file to get list of nice colors for elements
    read_color_file(); 
};

void Mesh::read_color_file(){
    std::ifstream color_list_file("src/colors.txt");
    std::string line;
    while (getline(color_list_file, line))
    {
        // one line is one color
        std::vector<std::string> rgb_string = misc::split_on(line, ',', false);
        std::array<float,3> rgb = {std::stof(rgb_string.at(0))/255,std::stof(rgb_string.at(1))/255,std::stof(rgb_string.at(2))/255};
        colors.push_back(rgb);
        if (color_list_file.eof() == true)
        {
            break;
        }
    } 
    return;
};


void Mesh::read_file(std::string filename){
    misc::append_newline_to_textfile(filename);
    // Create stream object
    std::ifstream input_file(filename);
    this->complete_filename = filename;
    std::vector<std::string> filename_split = misc::split_on(filename,'/',false);    
    this->filename = filename_split.at(filename_split.size()-1);
    std::string line;
    unsigned int row_counter = 0;
    while (getline(input_file, line))
    {
        row_counter++;
        if (misc::is_keyword(line) == true){
            std::string keyword = misc::split_on(line, ',', false).at(0);
            // extract a map of parameters and their values
            std::unordered_map<std::string,std::string> options = misc::options_map(line);
            // std::vector<std::string> keys;
            // for (std::unordered_map<std::string, std::string>::iterator it = options.begin(); it != options.end(); ++it)
            // {
            //     std::cout << it->first << "\n";
            // }

            // For things like nodes and elements data is typically on line after keyword
            if (keyword == "*SOLID SECTION" or keyword == "*SHELL SECTION")
            {
                add_pid(options);
            }     
            else if (keyword == "*MATERIAL")
            {
                // a complete material is typically described over several
                // lines so must make a complicated loop here..
                add_mid(options);
                bool inner_loop_keyword = true;
                // while (/* condition */)
                // {
                //     /* code */
                // }
                
            }
                   
            else if (keyword == "*NODE" or keyword == "*ELEMENT")
            {
                getline(input_file, line);
                row_counter++;
                bool inner_loop_keyword = true;
                while (inner_loop_keyword == true){
                // Ignore if it's a comment! Still on same keyword.
                    if (misc::is_comment(line) == false){
                        if (keyword == "*NODE")
                        {
                            add_node(line,options);
                        }
                        else if (keyword == "*ELEMENT")
                        {
                            add_element(line,options);
                        }
                    }
                    // Want to peek next line, if it's a keyword or empty line we break
                    // the while loop and start over!
                    unsigned int previous_pos = input_file.tellg();
                    getline(input_file, line);
                    // std::cout << line << ", is  keyword?"<< misc::is_keyword(line) << "\n";
                    if (misc::is_keyword(line) == true or line.empty() == true)
                    {
                        input_file.seekg(previous_pos);
                        inner_loop_keyword = false;
                    }
                }
                
                // end of inner loop
            }
            if (input_file.eof() == true)
            {
                // std::cout << line << "\n";
                break;
            }
            row_counter++;   
        }
    }       
    create_openGL_data();
    // Want to save max xyz positional values for the sake of zooming!
    for (auto& node: nodes)
    {
        if (node.x > max_x)
        {
            max_x = node.x;
        }
        if (node.y > max_y)
        {
            max_y = node.y;
        }
        if (node.z > max_z)
        {
            max_z = node.z;
        }
    }
    // Calculate CoG
    calculate_cog(this->nodes);
    // about();
};

void Mesh::add_pid(std::unordered_map<std::string, std::string> options){
    
    // Create random color
    // std::cout << colors.size() << "\n";
    std::array<float,3> color = colors.at(pid_counter);
    std::string pid_name = options["ELSET"];
    std::string mid_name = options["MATERIAL"];
    pidname2id[pid_name] = pid_counter;

    float alpha = 0.2;
    // Create new pid
    Pid temp_pid(pid_name, color,alpha, pid_counter);
    pids.push_back(temp_pid);
    pid_counter++;

};

void Mesh::create_openGL_data(){
    // Loop over each FE-element
    // std::cout << "element_counter == " << element_counter << "\n";
    // std::cout << "node_counter == " << node_counter << "\n";
    for (int local_element_id = 0; local_element_id < element_counter; local_element_id++)
    {
            // 4 nodes*3 (xyz) * 3 faces
            // std::cout << "local <-> global id" << "\n";
            // std::cout << local_element_id  << "<- element ->" << elements.at(local_element_id).global_id << "\n";
            // for (int i = 0; i < elements.at(local_element_id).connectivity.size(); i++)
            // {
            //     std::cout << elements.at(local_element_id).connectivity.at(i) << "<- node ->" << nodes.at(elements.at(local_element_id).connectivity.at(i)).global_id << "\n";
            //     std::cout << nodes.at(elements.at(local_element_id).connectivity.at(i)).local_id << "\n";
            // }
            // std::cout << "--" << "\n";
            std::vector<float> list_of_nodal_values;
            // Loop over all nodes from current FE element 
            try
            {
            for (int current_node = 0; current_node < elements.at(local_element_id).local_connectivity.size(); current_node++)
            {

                int current_node_local_id = elements.at(local_element_id).local_connectivity.at(current_node);
                // std::cout << "  current_node_local_id=" << current_node_local_id << " <-> " << elements.at(local_element_id).global_connectivity.at(current_node) << "\n";
                list_of_nodal_values.push_back(nodes.at(current_node_local_id).x);
                list_of_nodal_values.push_back(nodes.at(current_node_local_id).y);
                list_of_nodal_values.push_back(nodes.at(current_node_local_id).z);
            }
            }
            catch(const std::exception& e)
            {
                // std::cout << "--errol--" << "\n";
                // std::cout << "elements.at(local_element_id).local_connectivity.at(0)" << elements.at(local_element_id).local_connectivity.at(0) << "\n";
                // std::cout << "elements.at(local_element_id).local_connectivity.at(1)" << elements.at(local_element_id).local_connectivity.at(1) << "\n";
                // std::cout << "elements.at(local_element_id).local_connectivity.at(2)" << elements.at(local_element_id).local_connectivity.at(2) << "\n";
                // std::cout << "elements.at(local_element_id).local_connectivity.at(3)" << elements.at(local_element_id).local_connectivity.at(3) << "\n";
                // std::cout << "--errol--" << "\n";
                // std::cerr << e.what() << '\n';
                // std::cout << "--errol--" << "\n";
            }
            
            // Create triangles for current element
            try
            {
                create_faces(list_of_nodal_values, elements.at(local_element_id));
            }
            catch(const std::exception& e)
            {
                int i = elements.at(local_element_id).local_id;
                std::vector<std::string> temp;
                temp.push_back("a");
                temp.push_back("b");
                temp.push_back("c");
                temp.push_back("d");
                // std::cout << "Error creating vertex data for element " << elements.at(local_element_id).global_id << "\n";
                // std::cout << elements.at(i).type<< " element #" << elements.at(i).global_id << " (local id=" << elements.at(i).global_id << ")\n";
                // std::cout << " ";
                for (int j = 0; j < this->elements.at(i).global_connectivity.size(); j++)
                {
                    // std::cout << this->elements.at(i).global_connectivity.at(j) << ",";
                }
                // std::cout << " <-global - local->";
                for (int j = 0; j < this->elements.at(i).local_connectivity.size(); j++)
                {
                    // std::cout << this->elements.at(i).local_connectivity.at(j) << ",";
                }
                std::cout << "\n";
                for (int j = 0; j < this->elements.at(i).local_connectivity.size(); j++)
                {
                    // std::cout << " " << temp.at(j)<< ": " << this->elements.at(i).global_connectivity.at(j) << 
                    // " @ (" << nodes.at(this->elements.at(i).local_connectivity.at(j)).x << "," <<
                    // nodes.at(this->elements.at(i).local_connectivity.at(j)).y << "," <<
                    // nodes.at(this->elements.at(i).local_connectivity.at(j)).z << ")\n";
                }

            }
    }
    
};
void Mesh::create_faces(std::vector<float> &nodal_data_points, Element & current_element){
    // This function adds a number of nodes to the objects
    // std::vector faces_vertices which is a list of all the vertices
    // for all triangles. 
    // There are many types of different FE-elements that can be passed.
    // get Pid
    // std::cout 
    // Pid pid = pids.at(current_element.pid_id);
    // 
    // std::cout << "for element #" << current_element.global_id << "with PID " << current_element.pid_name << " "
    // << current_element.pid_id << " r=" << pids.at(current_element.pid_id).r << "\n";
    // std::cout << colors.size() << "\n";
    float r = colors.at(current_element.pid_id).at(0);
    float g = colors.at(current_element.pid_id).at(1);
    float b = colors.at(current_element.pid_id).at(2);
    Pid* pid_pointer = current_element.get_pid();
    float alpha = pid_pointer->alpha;

    
    // C3D4 element

    if (current_element.type == "C3D4"){
    // std::cout << "CP " << current_element.type << "\n";
        // 4 nodes, 4 faces, 3 positional floats per vertices
        //  d a b
        //  d b c
        //  d c a
        //  c b a     
        float a1 = nodal_data_points.at(0);
        float a2 = nodal_data_points.at(1);
        float a3 = nodal_data_points.at(2);
        float b1 = nodal_data_points.at(3);
        float b2 = nodal_data_points.at(4);
        float b3 = nodal_data_points.at(5);
        float c1 = nodal_data_points.at(6);
        float c2 = nodal_data_points.at(7);
        float c3 = nodal_data_points.at(8);
        float d1 = nodal_data_points.at(9);
        float d2 = nodal_data_points.at(10);
        float d3 = nodal_data_points.at(11);
        glm::vec3 normal1 = unit_normal(a1,a2,a3,b1,b2,b3,d1,d2,d3);
        glm::vec3 normal2 = unit_normal(d1,d2,d3,b1,b2,b3,c1,c2,c3);        
        glm::vec3 normal3 = unit_normal(d1,d2,d3,c1,c2,c3,a1,a2,a3);
        glm::vec3 normal4 = unit_normal(c1,c2,c3,b1,b2,b3,a1,a2,a3);
        std::vector<float> verts = {
            a1, a2, a3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
            b1, b2, b3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
            d1, d2, d3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
            d1, d2, d3,r,g,b,alpha,normal2.x, normal2.y, normal2.z,
            b1, b2, b3,r,g,b,alpha,normal2.x, normal2.y, normal2.z,
            c1, c2, c3,r,g,b,alpha,normal2.x, normal2.y, normal2.z,
            d1, d2, d3,r,g,b,alpha,normal3.x, normal3.y, normal3.z,
            c1, c2, c3,r,g,b,alpha,normal3.x, normal3.y, normal3.z,
            a1, a2, a3,r,g,b,alpha,normal3.x, normal3.y, normal3.z,
            c1, c2, c3,r,g,b,alpha,normal4.x, normal4.y, normal4.z,
            b1, b2, b3,r,g,b,alpha,normal4.x, normal4.y, normal4.z,
            a1, a2, a3,r,g,b,alpha,normal4.x, normal4.y, normal4.z,
        };
    
        for (int i = 0; i < verts.size(); i++)
        {
            // std::cout << "vertices data #"<< i << ": "<<verts.at(i) << "\n";
            this->faces_vertices.push_back(verts.at(i));
        }
    };
    if(current_element.type == "S3R")
    {
        float a1 = nodal_data_points.at(0);
        float a2 = nodal_data_points.at(1);
        float a3 = nodal_data_points.at(2);
        float b1 = nodal_data_points.at(3);
        float b2 = nodal_data_points.at(4);
        float b3 = nodal_data_points.at(5);
        float c1 = nodal_data_points.at(6);
        float c2 = nodal_data_points.at(7);
        float c3 = nodal_data_points.at(8);
        glm::vec3 normal1 = unit_normal(a1,a2,a3,b1,b2,b3,c1,c2,c3);
        std::vector<float> verts = {
            a1, a2, a3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
            b1, b2, b3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
            c1, c2, c3,r,g,b,alpha,normal1.x, normal1.y, normal1.z,
        };
        for (int i = 0; i < verts.size(); i++)
        {
            // std::cout << "vertices data #"<< i << ": "<<verts.at(i) << "\n";
            this->faces_vertices.push_back(verts.at(i));
        }
    }
    
    
        

};


void Mesh::add_element(std::string line,std::unordered_map<std::string,std::string> options){
    // these options need to be available to create an element
    std::string type = options["TYPE"];
    std::string pid_name = options["ELSET"];
    unsigned int pid_id = pidname2id[pid_name];
    // 
    std::vector<unsigned int> global_connectivity;
    std::vector<unsigned int> local_connectivity;
    std::vector<std::string> dataline_items = misc::split_on(line,',', false);
    unsigned int local_id    = element_counter;
    element_counter++;
    // std::cout << "added data line: " << line << "\n";
    unsigned int global_id   = std::stoi(dataline_items.at(0));
    // element will have at least 3 points, a 2D tria. 
    unsigned int p1          = std::stoi(dataline_items.at(1));
    unsigned int p2          = std::stoi(dataline_items.at(2));
    unsigned int p3          = std::stoi(dataline_items.at(3));
    
    if (type == "S3R")
    {
        global_connectivity = {p1, p2, p3};
        
    }
    else if (type == "C3D4")
    {
        unsigned int p4 = std::stoi(dataline_items.at(4));
        global_connectivity = {p1, p2, p3, p4};
        
    }
    else if (type == "quad")
    {
        unsigned int p4 = std::stoi(dataline_items.at(4));
        unsigned int p5 = std::stoi(dataline_items.at(5));
        unsigned int p6 = std::stoi(dataline_items.at(6));
        unsigned int p7 = std::stoi(dataline_items.at(7));
        unsigned int p8 = std::stoi(dataline_items.at(8));
        global_connectivity = {p1, p2, p3, p4, p5, p6, p7, p8};
    }
    else{
        std::cout << "Element type " << type << " not supported." << "\n";
        return;
    }
    for (int i = 0; i < global_connectivity.size(); i++)
    {
        local_connectivity.push_back(global2local_node[global_connectivity.at(i)]);
    }
    global2local_element[global_id] = local_id;
    Pid* pid_pointer = &pids.at(pid_id);
    // create temp element to add to the Mesh object
    Element temp(type,
                 local_id,
                 global_id,
                 global_connectivity,
                 local_connectivity,
                 pid_name,
                 pid_id,
                 pid_pointer);
    temp.options = options;
    // push back the temp element to the vector of elements
    // belonging to the Mesh
    elements.push_back(temp);
    };
void Mesh::add_node(std::string line,std::unordered_map<std::string, std::string> options){
    // aba docs:
    // Data line to define the node
    // Node number.
    // First coordinate of the node.
    // Second coordinate of the node.
    // Third coordinate of the node.
    // First direction cosine of the normal at the node (optional).
    // Second direction cosine of the normal at the node (optional). For nodes entered in a cylindrical or spherical system, this entry is an angle given in degrees.
    // Third direction cosine of the normal at the node (optional). For nodes entered in a spherical system, this entry is an angle given in degrees.
    
    // another node added to the Mesh!
    std::vector<std::string> dataline_items = misc::split_on(line,',',false);
    // Loop over data items.
    // TODO: add support for more node options like coordinate system and stuff
    try{
    unsigned int temp_local_id = node_counter;
    this->node_counter++;
    unsigned int temp_global_id = std::stoi(dataline_items.at(0));
    float                     x = std::stof(dataline_items.at(1));
    float                     y = std::stof(dataline_items.at(2));
    float                     z = std::stof(dataline_items.at(3));
    global2local_node[temp_global_id] = temp_local_id;
    Node temp(temp_local_id, temp_global_id, x, y, z);
    this->nodes.push_back(temp);
    // std::cout << "added data line: " << line << "\n";
    }
    catch (std::out_of_range e)
    {
        // std::cout << "a" << "\n";
    }
    catch (std::invalid_argument e)
    {
        // std::cout << "bb" << "\n";
    }
};

void Mesh::about(){
    // std::cout << "**********" << "\n";
    // std::cout << "Nodes:" << node_counter << "\n";

    // std::cout << "Elements:" << this->elements.size() << "\n";
    std::vector<std::string> temp;
    temp.push_back("a");
    temp.push_back("b");
    temp.push_back("c");
    temp.push_back("d");
    for (unsigned int i = 0; i < this->elements.size(); i++)
    {
        std::cout << elements.at(i).type<< " element #" << elements.at(i).global_id << " (local id=" << elements.at(i).global_id << ")\n";
        std::cout << " ";
        for (int j = 0; j < this->elements.at(i).global_connectivity.size(); j++)
        {
            std::cout << this->elements.at(i).global_connectivity.at(j) << ",";
        }
        std::cout << " <-global - local->";
        for (int j = 0; j < this->elements.at(i).local_connectivity.size(); j++)
        {
            std::cout << this->elements.at(i).local_connectivity.at(j) << ",";
        }
        std::cout << "\n";
        for (int j = 0; j < this->elements.at(i).local_connectivity.size(); j++)
        {
            std::cout << " " << temp.at(j)<< ": " << this->elements.at(i).global_connectivity.at(j) << 
            " @ (" << nodes.at(this->elements.at(i).local_connectivity.at(j)).x << "," <<
            nodes.at(this->elements.at(i).local_connectivity.at(j)).y << "," <<
            nodes.at(this->elements.at(i).local_connectivity.at(j)).z << ")\n";
        }
        if (elements.at(i).type == "C3D4")
        {   
            std::cout << " faces_vertices.size()="<< faces_vertices.size() << " (element_counter*9=" << element_counter*9 <<  ")\n  ";
            std::cout << " face #1 a, b, d:" << "\n  ";
            
            for (int j = 0; j < 3; j++)
            {
                std::cout << "(" << faces_vertices.at((9*i)+j) << "," << faces_vertices.at((9*i)+j+1) << "," << faces_vertices.at((9*i)+j+2) << ")," << "\n  ";
            }
            
                // 
                // for (int k = 0; k < 3; k++)
                // {
                //     std::cout << ;
                // }
                // std::cout << "),";
            // std::cout << " face #2 d, b, c" 
            // // std::cout << " face #3 d, c, a" 
            // std::cout << " face #4 c, b, a" 
        }
        


        std::cout << "\n";       
        std::cout << "------" << "\n";
    }
    
    
};

Mesh::~Mesh(){};

Node::Node(unsigned int local_node_id,unsigned int global_node_id, float x, float y, float z)
{
    try
    {
    this->x = x;
    this->y = y;
    this->z = z;
    this->local_id = local_node_id;
    this->global_id = global_node_id;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error creating node: " << e.what() << '\n';
    }
    
};

Node::~Node(){};

Element::Element(std::string type,
                 unsigned int local_id,
                 unsigned int global_id,
                 std::vector<unsigned int> global_connectivity,
                 std::vector<unsigned int> local_connectivity,
                 std::string pid_name,
                 unsigned int pid_id,
                 Pid* pid){
    try
    {
    this->global_id = global_id;
    this->local_id = local_id;
    this->global_connectivity = global_connectivity;
    this->local_connectivity = local_connectivity;
    this->type = type;
    this->pid_name = pid_name;
    this->pid_id = pid_id;
    this->pid = pid;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error creating element: "<< e.what() << '\n';
    }
    

};



glm::vec3 Mesh::unit_normal(float &p1, float &p2, float &p3, float &p4, float &p5, float &p6, float &p7, float &p8, float &p9){
    glm::vec3 v1(p1,p2,p3);
    glm::vec3 v2(p4,p5,p6);
    glm::vec3 v3(p7,p8,p9);
    glm::vec3 normal        = glm::triangleNormal(v1,v2,v3);
    glm::vec3 unit_normal   = glm::normalize(normal);
    return unit_normal;
};

Mid::Mid(int id, std::string name){};

Mid::~Mid(){};

Element::~Element(){};

Pid::Pid(std::string name, std::array<float, 3> color,float alpha, unsigned int id){
    try
    {
    this->id    = id;
    this->name  = name;
    this->color = color;
    this->r     = color.at(0);
    this->g     = color.at(1);
    this->b     = color.at(2);
    this->alpha = alpha;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error creating PID: "<<e.what() << '\n';
    }
    
}

Pid::~Pid(){
}