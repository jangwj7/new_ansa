#include <map>
#include <glad/glad.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <string>
#include "shaders/shader.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class Text
{
private:
    unsigned int VBO,VAO;
    // shader for text. hardcoded shaders lol
    Shader text_shader = Shader("src/shaders/text.vs","src/shaders/text.fs");
    // font
    
    // one struct for each letter
    struct Character{
        unsigned int texture_id;
        glm::ivec2 size;
        glm::ivec2 bearing;
        int advance;
    };
    // map of char 2 Character structs
    std::map<char,Character> Characters;

public:
    // show text
    void render_text(std::string text,float x, float y, float scale,glm::vec3 color);
    Text(std::string font,int fontsize);
    ~Text();
};





