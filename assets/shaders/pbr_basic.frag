#version 410 core

in vec3 fWorldPos;
in vec3 fNormal;

out vec4 FragColor;

struct LightProperties
{
    int isEnabled;
    int isLocal;
    int isSpot;

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

// material parameters
struct MaterialProperties {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

const int MAX_LIGHTS = 10;
uniform vec3 uEyePosition;
uniform MaterialProperties uMaterial;
uniform LightProperties uLight[MAX_LIGHTS];

const float PI = 3.14159265359;
const float EPSILON = 0.000001;

// Specular D: GGX/Trowbridge-Reitz
float D_GGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
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
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < MAX_LIGHTS; ++i) 
    {
        if (uLight[i].isEnabled == 0)
            continue;

        float attenuation = 1.0;
        vec3 L = normalize(-uLight[i].position);

        if (uLight[i].isLocal == 1) {
            L = normalize(uLight[i].position - fWorldPos);

            float dist = length(uLight[i].position - fWorldPos);
            attenuation = 1.0 / ( uLight[i].constantAttenuation 
                                + uLight[i].linearAttenuation * dist 
                                + uLight[i].quadraticAttenuation * dist * dist);
            
            if (uLight[i].isSpot == 1) {
                float spotCos = dot(L, -uLight[i].coneDirection);
                if (spotCos < uLight[i].spotCosCutoff)
                    attenuation = 0.0;
                else
                    attenuation *= pow(spotCos, uLight[i].spotExponent);
            }
        }

        vec3 H = normalize(L + V);
        vec3 radiance = uLight[i].color * attenuation;

        // Cook-Torrance BRDF
        float NDF = D_GGX(N, H, uMaterial.roughness);   
        float G   = G_Smith(N, V, L, uMaterial.roughness);      
        vec3  F   = F_Schlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3  numerator   = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3  specular    = numerator / denominator;
        
        // Energy conservation: kS + KD == 1.0
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        // No diffuse light if completely metallic
        kD *= 1.0 - uMaterial.metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += 10 * (kD * uMaterial.albedo / PI + specular) * radiance * NdotL;
    }   
    
    // Ambient light to prevent the scene from getting too dark
    vec3 ambient = vec3(0.03) * uMaterial.albedo * uMaterial.ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}