#version 330 core
// pipe input, see mesh::create_faces
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColor;
layout(location = 2) in float VertexAlpha;
layout(location = 3) in vec3 VertexNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out float Alpha;


// globals, available for all vertices
uniform mat4 ModelMatrix,ProjectionMatrix,ViewMatrix;

void main()
{
    FragPos = vec3(ModelMatrix * vec4(VertexPosition, 1.0));
    Normal = mat3(transpose(inverse(ModelMatrix))) * VertexNormal;
    Color = VertexColor;
    Alpha = VertexAlpha;
    gl_Position     = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(VertexPosition, 1.0);
}
