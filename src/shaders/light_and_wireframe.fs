#version 330 core
//
//

in vec3 Normal_;  
in vec3 FragPos_;  
in vec3 Color_;
in float Alpha_;

noperspective in vec3 distance_to_opposing_edge;
out vec4 FragColor;

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform bool hide_show_mesh;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal_);
    vec3 lightDir = normalize(lightPos - FragPos_);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos_);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    // black line color!
    vec3 resulting_color = (ambient + diffuse + specular) * Color_;
    if(hide_show_mesh==true){
        vec3 line_color = vec3(0,0,0);
        float d = min(distance_to_opposing_edge[0],min(distance_to_opposing_edge[1],distance_to_opposing_edge[2]));
        float I = 1.0 - step(0.003, d);
        FragColor = vec4(I*line_color + (1-I)*resulting_color,1.0);
    }
    else
    {
        FragColor = vec4(resulting_color,1.0);
    }
    // To turn off wireframe:
    // I = 0;
    // FragColor.rgb = I*line_color + (1-I)*resulting_color;
    // FragColor.a = 0.2;
    // if(gl_FrontFacing)
        // FragColor = vec4(0.69,0.69,0.69,1.0);
    // else

}