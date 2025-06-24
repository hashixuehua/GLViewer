#version 450 core
out vec4 FragColor;

//in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

//uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec4 objectColor;

void main()
{
    FragColor = vec4(0.0, 1.0, 1.0, 1.0);
}