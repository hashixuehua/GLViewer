#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform int sampleTexture = 1;
uniform sampler2D texture_diffuse1;
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
	
	if (sampleTexture == 1)
	{
        //FragColor = vec4(result, 0.3);
        //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);//texture(texture_diffuse1, TexCoords);
	    
	    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture_diffuse1, TexCoords).r);
        //FragColor = vec4(result, 1.0) * sampled;
	    
	    FragColor = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0);
	    //FragColor = texture(texture_diffuse1, TexCoords);
	}
	else
	{
        FragColor = vec4(result, objectColor.a);
	}
}