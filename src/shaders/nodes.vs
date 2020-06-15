#version 330 core
layout(location = 0) in vec3 VertexPosition;


// globals, available for all vertices
uniform mat4 ModelMatrix,ProjectionMatrix,ViewMatrix;

void main()
{
    gl_Position  = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(VertexPosition, 1.0);
    // TODO: set uniform
    gl_PointSize = 10;  
}
