#version 450 core
out vec4 FragColor;

//in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

//uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec4 objectColor;
uniform vec4 singleColor;

void main()
{
    FragColor = singleColor;
}