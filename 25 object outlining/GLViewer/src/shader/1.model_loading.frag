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
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * objectColor.rgb;
    FragColor = vec4(result, objectColor.a);
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);//texture(texture_diffuse1, TexCoords);
}