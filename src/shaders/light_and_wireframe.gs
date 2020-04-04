#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 Normal_;  
out vec3 FragPos_;
out vec3 Color_;
out float Alpha_;

in vec3 Normal[];  
in vec3 FragPos[];  
in vec3 Color[];
in float Alpha[];
noperspective out vec3 distance_to_opposing_edge;

uniform mat4 ViewMatrix;

void main()
{
	// Transform each vertex into viewport space
	vec3 P0 = vec3(ViewMatrix*(gl_in[0].gl_Position/gl_in[0].gl_Position.w));
	vec3 P1 = vec3(ViewMatrix*(gl_in[1].gl_Position/gl_in[1].gl_Position.w));
	vec3 P2 = vec3(ViewMatrix*(gl_in[2].gl_Position/gl_in[2].gl_Position.w));
	// Find altitudes
	float e0 = length(P1-P2);
	float e1 = length(P2-P0);
	float e2 = length(P1-P0);
	float alpha_angle = acos( (e1*e1 + e2*e2 - e0*e0) / (2.0*e1*e2) );
	float beta = acos( (e0*e0 + e2*e2 - e1*e1) / (2.0*e0*e2) );
	float h0 = abs(e2*sin(beta));
	float h1 = abs(e2*sin(alpha_angle));
	float h2 = abs(e1*sin(alpha_angle));
	// Output the same triangle to fragment shader but add the 3 distances 
	Normal_ = Normal[0];  
	FragPos_ = FragPos[0];
	Color_ = Color[0]; 
	Alpha_ = Alpha[0];
	distance_to_opposing_edge = vec3(h0,0,0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	Normal_ = Normal[1];  
	FragPos_ = FragPos[1];
	Color_ = Color[1]; 
	Alpha_ = Alpha[1];
	distance_to_opposing_edge = vec3(0,h1,0);
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	Normal_ = Normal[2];  
	FragPos_ = FragPos[2];
	Color_ = Color[2];
	Alpha_ = Alpha[2]; 
	distance_to_opposing_edge = vec3(0,0,h2);
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

