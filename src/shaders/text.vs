#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 tex;
uniform mat4 ProjectionMatrix,ModelMatrix,ViewMatrix;
out vec2 TexCoords;

void main()
{
    // gl_Position = ModelMatrix*ProjectionMatrix* vec4(vertex.x,vertex.y,vertex.z, 1.0);
    gl_Position     =  ModelMatrix * ProjectionMatrix*vec4(vertex, 1.0);
    TexCoords       = tex;
} 

