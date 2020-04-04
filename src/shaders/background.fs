#version 330 core
uniform vec3 top_color;
uniform vec3 bottom_color;
uniform vec2 resolution;
out vec4 FragColor;

void main()
{
    // vec3 bottom_color = vec3(0,0.0784,0.141);
    // vec3 top_color = vec3(0.2,0.278,0.34);
    // vec3 bottom_color = vec3(0.3,0.3,0.3);
    // vec3 top_color = vec3(1.0,1.0,1.0);
    vec2 xy = gl_FragCoord.xy;
    // vec2 resolution = vec2(600,800);

    xy.x = xy.y / resolution.x;
    xy.y = xy.x / resolution.y;
	FragColor = vec4(bottom_color * (1.0 - xy.x) + 1.0*top_color * xy.x,1.0);
}