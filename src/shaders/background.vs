#version 330 core
void main()
{
    uint idx = uint(gl_VertexID);
    gl_Position = vec4( idx & 1U, idx >> 1U, 0.0, 0.5 ) * 4.0 - 1.0;
}
