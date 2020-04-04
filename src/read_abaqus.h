#pragma once
#include "../glm/glm.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
class Node
{
private:
public:
    // Node has id and positional data
    float x, y, z;
    unsigned int global_id, local_id;
    Node(unsigned int local_id,unsigned int global_id, float x, float y, float z);
    ~Node();
};

class Mid
{
private:
    /* data */
public:
    std::array<float, 3> color;
    unsigned int id;
    std::string name;
    
    Mid(int id, std::string name);
    std::unordered_map<std::string,std::string> options;
    ~Mid();
};
class Pid
{
private:
    Mid* mid;
    // 
public:
    Mid* get_mid(){return this->mid;};
    float r,g,b;
    std::array<float,3> color;
    float alpha;
    int mid_id;
    unsigned int id;
    std::string name;
    Pid(std::string name, std::array<float, 3> color,float alpha,unsigned int);
    std::unordered_map<std::string,std::string> options;
    ~Pid();
};




class Element{
private:
    Pid* pid;
public:
    std::unordered_map<std::string,std::string> options;
    Pid* get_pid(){return this->pid;}
    unsigned int pid_id,mid_id;
    std::string type;
    std::string pid_name;
    unsigned int global_id,local_id,vertices;
    // connectivity in local Node id's
    std::vector<unsigned int> global_connectivity;
    std::vector<unsigned int> local_connectivity;
    Element(std::string type,
            unsigned int local_id,
            unsigned int global_id,
            std::vector<unsigned int> global_connectivity,
            std::vector<unsigned int> local_connectivity,
            std::string pid_name,
            unsigned int pid_id,
            Pid*);
    ~Element();
};







class Mesh
{
private:
    // static counters, if we should decide to
    // add another Mesh object they will still
    // be available
    static unsigned int node_counter;
    static unsigned int element_counter;
    static unsigned int pid_counter;
    static unsigned int mid_counter;

    // vector of colors
    void read_color_file();
    std::string filename;
    std::string complete_filename;
    std::vector<std::array<float,3>> colors;
    // array of nodes
    std::vector<Node> nodes;
    std::vector<Element> elements;
    // array of PID's belonging to the Mesh
    std::vector<Pid> pids;

    // map of PID name to PID number
    std::unordered_map<std::string,unsigned int> pidname2id;
    // map of MID name to MID number
    std::unordered_map<std::string,unsigned int> midname2id;
    // If elements are created before the PID we want to create a boilerplate PID with the
    // specified name
    std::vector<std::string> pid_names; 
    // array of MID's belonging to the Mesh
    std::vector<Mid> mids;
    // map from global id to local id
    // array of elements and their connectivity
    // Method to add Node
    void add_node(std::string line,std::unordered_map<std::string, std::string> options);
    void add_element(std::string line,std::unordered_map<std::string, std::string> options);
    void add_pid(std::unordered_map<std::string, std::string> options);
    void add_mid(std::unordered_map<std::string, std::string> options);
    void create_faces(std::vector<float> &,Element &);



    // function to return aba Node  id from local id
    unsigned int get_abaqus_node_id(unsigned int local_id);
    // function to return aba elem id from local id
    unsigned int get_abaqus_element_id(unsigned int local_id);
    void calculate_cog(std::vector<Node>&);

    void about();
    void create_openGL_data();
    glm::vec3 unit_normal(float &p1, float &p2, float &p3, float &p4, float &p5, float &p6, float &p7, float &p8, float &p9);

public:
    float cog_x=0;
    float cog_y=0;
    float cog_z=0;
    // Want to save max values of position so that we can set the camera nicely.
    // init to zero but will update later! safer to have as zero than nothing?
    float max_x = 0;
    float max_y = 0;
    float max_z = 0;


    unsigned int get_pid_counter(){return pid_counter;};
    std::vector<glm::vec3> world_space_positions;
    std::unordered_map<unsigned int,unsigned int>  global2local_node;
    std::unordered_map<unsigned int, unsigned int> global2local_element;
    std::vector<float> faces_vertices;
    // getters and setters
    unsigned long get_number_of_elements(){return this->elements.size();}
    Element* get_element(unsigned long local_element_id){return &this->elements.at(local_element_id);}
    unsigned long get_number_of_nodes(){return this->nodes.size();}
    Node* get_node(unsigned long local_node_id){return &this->nodes.at(local_node_id);}
    Pid* get_pid(unsigned int pid_id) {return &this->pids.at(pid_id);}
    Mid* get_mid(unsigned int mid_id) {return &this->mids.at(mid_id);}
    std::string get_filename() const {return this->filename;};
    std::string get_complete_filename() const {return this->complete_filename;};
    void update_vertices(Pid* pid);
    void update_vertices(Element* element);
    // map with edges to adjacent elements
    // std::unordered_map

    Mesh();
    void read_file(std::string filename);
    std::vector<unsigned int> Mesh_connectivity;
    ~Mesh();
};