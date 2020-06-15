#version 330 core
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColor;
layout(location = 2) in float VertexAlpha;
layout(location = 3) in vec3 VertexNormal;

// globals, available for all vertices
uniform mat4 ModelMatrix,ProjectionMatrix,ViewMatrix;

out vec3 Color;
// out vec3 FragPos;

void main()
{
    Color       = VertexColor;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(VertexPosition, 1.0);
}
