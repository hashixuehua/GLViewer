#version 450 core
out vec4 FragColor;
uniform vec4 objectColor;

void main()
{    
    FragColor = objectColor;
}