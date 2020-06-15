#version 330 core
uniform vec3 top_color;
uniform vec3 bottom_color;
uniform vec2 resolution;
out vec4 FragColor;

void main()
{
    vec2 xy = gl_FragCoord.xy;
    xy.x = xy.y / resolution.x;
    xy.y = xy.x / resolution.y;
	FragColor = vec4(bottom_color * (1.0 - xy.x) + 1.0*top_color * xy.x,1.0);
}