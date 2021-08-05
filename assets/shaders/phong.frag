#version 410 core

in vec3 fNormal;  
in vec3 fWorldPos;  

out vec4 FragColor;

struct LightProperties
{
    int isEnabled;
    int isLocal;
    // position/direction
    vec3 position;
    // colors/intensities
    vec3 color;
    vec3 ambient;
    // for spot light
    vec3 coneDirection;
    float spotExponent;
    float spotCosCutoff;
    // for local lights
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 uEyePosition; 
uniform LightProperties uLight;
uniform MaterialProperties uMaterial;

void main()
{
    // ambient
    vec3 cAmbient = uLight.isEnabled * uLight.ambient * uMaterial.ambient;
  	
    // diffuse 
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(uLight.position - fWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 cDiffuse = uLight.isEnabled * uLight.color * (diff * uMaterial.diffuse);
    
    // specular
    vec3 viewDir = normalize(uEyePosition - fWorldPos);
    vec3 halfVec = normalize(lightDir + viewDir); 
    float spec = pow(max(dot(norm, halfVec), 0.0), uMaterial.shininess);
    vec3 cSpecular = uLight.isEnabled * uLight.color * (spec * uMaterial.specular);  
        
    vec3 result = cAmbient + cDiffuse + cSpecular;
    FragColor = vec4(result, 1.0);
}