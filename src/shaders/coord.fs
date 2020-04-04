#version 330 core
//
//
in vec3 Color;
// noperspective in vec3 distance_to_opposing_edge;
out vec4 FragColor;

void main()
{
    // black line color!
    // vec3 line_color = vec3(0,0,0);
    // float d = min(distance_to_opposing_edge[0],min(distance_to_opposing_edge[1],distance_to_opposing_edge[2]));
    // float I = 1.0 - step(0.003, d);
    // To turn off wireframe:
    // I = 0;
    // FragColor.rgb = I*line_color + (1-I)*resulting_color;
    // FragColor.a = 0.2;
    // FragColor = vec4(I*line_color + (1-I)*Color,1.0);
    FragColor = vec4(Color,1.0);
}