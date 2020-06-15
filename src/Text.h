#ifndef TEXT_H
#define TEXT_H
#include "shaders/shader.h"
#include <map>
#include <glad/glad.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

// one struct for each letter
struct Character{
    unsigned int texture_id;
    glm::ivec2 size;
    glm::ivec2 bearing;
    int advance;
};
class Text
{
private:
    // shader for text. hardcoded shaders lol
    // "shaders/text.vs";
    // "shaders/text.fs";
    std::string vertex_file;
    std::string fragment_file;

    unsigned int VBO,VAO;
    // map of char 2 Character structs
    std::map<char,Character> Characters;

public:
    Shader* text_shader;
    // show text
    void render_text(std::string text,float x, float y,float z, float scale,glm::vec3 color);
    Text(std::string font,
        int fontsize,
        std::string vertex_file,
        std::string fragment_file);
    ~Text();
};
#endif