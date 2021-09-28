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

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

const int MAX_LIGHTS = 10;
uniform vec3 uEyePosition;
uniform MaterialProperties uMaterial;
uniform LightProperties uLight[MAX_LIGHTS];

void main()
{
    vec3 RGB = vec3(0.0);

    vec3 N = normalize(fNormal);
    vec3 V = normalize(uEyePosition - fWorldPos);

    for (int i = 0; i < MAX_LIGHTS; i++) {
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
        float diff = max(0.0, dot(N, L));
        float spec = max(0.0, dot(N, H));

        if (diff == 0.0)
            spec = 0.0;
        else
            spec = pow(spec, uMaterial.shininess);

        // Accumulate all the lights' effects
        RGB += uLight[i].ambient * uMaterial.ambient * attenuation;
        RGB += uLight[i].color * uMaterial.diffuse  * diff * attenuation;
        RGB += uLight[i].color * uMaterial.specular * spec * attenuation;
    }

    FragColor = vec4(min(RGB, vec3(1.0)), 1.0);
}