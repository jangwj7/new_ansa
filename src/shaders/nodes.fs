#version 330 core
//
//
out vec4 FragColor;

// texture sampler
uniform sampler2D tex;

void main()
{
    vec4 tex_ = texture(tex, gl_PointCoord);
    if(tex_.a < 0){
        discard;
    }
    FragColor = tex_;
}