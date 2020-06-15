#ifndef COMMON_H
#define COMMON_H
#include <vector>
#include <string>
#include "stb_image.h"
namespace common
{
    // read image file to save as texture
    int generate_texture(std::string,bool alpha = true);
    // read generic vertex file
    std::vector<float> read_vertex_file(std::string input_file);

} 
#endif