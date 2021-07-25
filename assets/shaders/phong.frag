#version 410 core

in vec3 fNormal;  
in vec3 fWorldPos;  

out vec4 FragColor;

uniform int uShininess;
uniform vec3 uLightPosition; 
uniform vec3 uEyePosition; 
uniform vec3 uLightColor;
uniform vec3 uObjectColor;

void main()
{
    // ambient
    float Ia = 0.1;
    vec3 ambient = Ia * uLightColor;
  	
    // diffuse 
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(uLightPosition - fWorldPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * uLightColor;
    
    // specular
    float Is = 0.5;
    vec3 viewDir = normalize(uEyePosition - fWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 specular = Is * pow(max(dot(viewDir, reflectDir), 0.0), uShininess) * uLightColor;  
        
    vec3 result = (ambient + diffuse + specular) * uObjectColor;
    FragColor = vec4(result, 1.0);
} 