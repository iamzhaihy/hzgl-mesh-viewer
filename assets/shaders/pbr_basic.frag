#version 410 core

in vec3 fWorldPos;
in vec3 fNormal;

out vec4 FragColor;

struct LightProperties
{
    vec3 position;
    vec3 color;
};

struct MaterialProperties
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

uniform vec3 uEyePosition;
uniform LightProperties uLight;
uniform MaterialProperties uMaterial;

const float PI = 3.14159265359;
const float EPSILON = 0.000001;
const float LINTENSITY = 300;

// Specular D: GGX/Trowbridge-Reitz
float D_GGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float NdotH = max(dot(N, H), 0.0);

    float nume = a * a;
    float denom = (NdotH * NdotH * (a * a - 1.0) + 1.0);
    denom = PI * denom * denom;

    // prevent division by zero
    return nume / max(denom, EPSILON);
}

// Specular G: Schlick
float G_Schlick(vec3 N, vec3 V, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float NdotV = max(dot(N, V), 0.0);

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// Specular G: Smith
float G_Smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float ggx2 = G_Schlick(N, V, roughness);
    float ggx1 = G_Schlick(N, V, roughness);

    return ggx1 * ggx2;
}

// Specular F: Schlick
vec3 F_Schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{	
    vec3 N = normalize(fNormal);
    vec3 V = normalize(uEyePosition - fWorldPos);

    // F0 = reflectance at fNormal incidence
    //    - dia-electric: 0.04
    //    - metal: the albedo color  
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, uMaterial.albedo, uMaterial.metallic);

    // calculate per-light radiance
    vec3 L = normalize(uLight.position - fWorldPos);
    vec3 H = normalize(V + L);

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float distance = length(uLight.position - fWorldPos);
    float attenuation = 1.0 / (distance * distance);

    vec3 radiance = uLight.color * LINTENSITY * attenuation;

    // Cook-Torrance BRDF
    float NDF = D_GGX(N, H, uMaterial.roughness);   
    float G = G_Smith(N, V, L, uMaterial.roughness);      
    vec3 F = F_Schlick(clamp(dot(H, V), 0.0, 1.0), F0);
        
    vec3 nume = NDF * G * F; 
    float denom = 4 * NdotV* NdotL;
    vec3 specular = nume / max(denom, EPSILON);
    
    // Energy conservation: kS + KD == 1.0
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    // No diffuse light if completely metallic
    kD *= 1.0 - uMaterial.metallic;	    

    vec3 Lo = (kD * uMaterial.albedo / PI * specular) * radiance * NdotL;
    
    // Ambient light to prevent the scene from getting too dark
    vec3 ambient = vec3(0.05) * uMaterial.albedo * uMaterial.ao;

    vec3 color = ambient + Lo;

    // Tonemapping
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}