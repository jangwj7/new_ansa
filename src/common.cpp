#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "common.h"
#include "misc_string_functions.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
int common::generate_texture(std::string filename,bool alpha){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    // set texture filtering parameters
    int width, height, nrChannels;
    // std::string fileName = "src/resources/show_hide_nodes.png";
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0); 
    if (data)
    {
        if (alpha == false)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
                
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return texture;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

};


std::vector<float> common::read_vertex_file(std::string input_file){
    // takes file on format x,y,z,r,g,b,alpha,normal.x,normal.y,normal.z
    std::vector<float> output;
    std::ifstream file;
    std::string temp;
	file.open(input_file);
    while(!file.eof()) // To get you all the lines.
    {
        getline(file,temp); // Saves the line in STRING.
        std::vector<std::string> datum = misc::split_on(temp,',',false);
        for (int i = 0; i < datum.size(); i++)
        {
            try
            {
            float data = std::stof(datum.at(i));
            output.push_back(data);
            }
            catch(const std::exception& e)
            {
                // std::cerr << "empty line, exit scam" << '\n';
            }
            
        }
        
    }
	file.close();
    return output;
};