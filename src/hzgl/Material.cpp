#include "Material.hpp"

hzgl::Material::Material(MaterialType t): type(t) 
{
    if (type == HZGL_PHONG_MATERIAL)
        InitPhong();
    else if (type == HZGL_PBR_MATERIAL)
        InitPBR();
}

void hzgl::Material::InitPhong(const glm::vec3 &pAmbient, const glm::vec3 &pDiffuse, const glm::vec3 &pSpecular, float pShininess) 
{
    ambient = pAmbient;
    diffuse = pDiffuse;
    specular = pSpecular;
    shininess = pShininess;
}

void hzgl::Material::InitPBR(const glm::vec3 &pAlbedo, float pMetallic, float pRoughness, float pAo) 
{
    albedo = pAlbedo;
    metallic = pMetallic;
    roughness = pRoughness;
    ao = pAo;
}

static GLuint hzglLoc(GLuint program, const std::string& uName)
{
    return glGetUniformLocation(program, uName.c_str());
}

std::string hzgl::MaterialTypeName(MaterialType type)
{
    std::string matType;

    switch (type)
    {
        case HZGL_PHONG_MATERIAL:
            matType = "Phong Material";
            break;
        case HZGL_PBR_MATERIAL:
            matType = "PBR Material";
            break;
        default:
            matType = "Unknown type";
            break;
    }

    return matType;
}

// references:
//   - http://devernay.free.fr/cours/opengl/materials.html
//   - https://www.opengl.org/archives/resources/code/samples/sig99/advanced99/notes/node153.html
//   - http://web.archive.org/web/20100725103839/http://www.cs.utk.edu/~kuck/materials_ogl.htm
static void hzglFillPhongMaterial(hzgl::Material &material, const std::string &name)
{
    if (name == "emerald")
    {
        material.ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f);
        material.diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
        material.specular = glm::vec3(0.633f, 0.727811f, 0.633f);
        material.shininess = 76.8f;
    }
    else if (name == "jade")
    {
        material.ambient = glm::vec3(0.135f, 0.2225f, 0.1575f);
        material.diffuse = glm::vec3(0.54f, 0.89f, 0.63f);
        material.specular = glm::vec3(0.316228f, 0.316228f, 0.316228f);
        material.shininess = 12.8f;
    }
    else if (name == "obsidian")
    {
        material.ambient = glm::vec3(0.05375f, 0.05f, 0.06625f);
        material.diffuse = glm::vec3(0.18275f, 0.17f, 0.22525f);
        material.specular = glm::vec3(0.332741f, 0.328634f, 0.346435f);
        material.shininess = 38.4f;
    }
    else if (name == "pearl")
    {
        material.ambient = glm::vec3(0.25f, 0.20725f, 0.20725f);
        material.diffuse = glm::vec3(1.0f, 0.829f, 0.829f);
        material.specular = glm::vec3(0.296648f, 0.296648f, 0.296648f);
        material.shininess = 11.264f;
    }
    else if (name == "ruby")
    {
        material.ambient = glm::vec3(0.1745f, 0.01175f, 0.01175f);
        material.diffuse = glm::vec3(0.61424f, 0.04136f, 0.04136f);
        material.specular = glm::vec3(0.727811f, 0.626959f, 0.626959f);
        material.shininess = 76.8f;
    }
    else if (name == "turquoise")
    {
        material.ambient = glm::vec3(0.1f, 0.18725f, 0.1745f);
        material.diffuse = glm::vec3(0.396f, 0.74151f, 0.69102f);
        material.specular = glm::vec3(0.297254f, 0.30829f, 0.306678f);
        material.shininess = 12.8f;
    }
    else if (name == "brass")
    {
        material.ambient = glm::vec3(0.329412f, 0.223529f, 0.027451f);
        material.diffuse = glm::vec3(0.780392f, 0.568627f, 0.113725f);
        material.specular = glm::vec3(0.992157f, 0.941176f, 0.807843f);
        material.shininess = 27.89743616f;
    }
    else if (name == "bronze")
    {
        material.ambient = glm::vec3(0.2125f, 0.1275f, 0.054f);
        material.diffuse = glm::vec3(0.714f, 0.4284f, 0.18144f);
        material.specular = glm::vec3(0.393548f, 0.271906f, 0.166721f);
        material.shininess = 25.6f;
    }
    else if (name == "chrome")
    {
        material.ambient = glm::vec3(0.25f, 0.25f, 0.25f);
        material.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        material.specular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
        material.shininess = 76.8f;
    }
    else if (name == "copper")
    {
        material.ambient = glm::vec3(0.19125f, 0.0735f, 0.0225f);
        material.diffuse = glm::vec3(0.7038f, 0.27048f, 0.0828f);
        material.specular = glm::vec3(0.256777f, 0.137622f, 0.086014f);
        material.shininess = 12.8f;
    }
    else if (name == "gold")
    {
        material.ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
        material.diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
        material.specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
        material.shininess = 51.2f;
    }
    else if (name == "silver")
    {
        material.ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
        material.diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
        material.specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
        material.shininess = 51.2f;
    }
    else if (name == "black plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
        material.specular = glm::vec3(0.50f, 0.50f, 0.50f);
        material.shininess = 32.0f;
    }
    else if (name == "cyan plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.1f, 0.06f);
        material.diffuse = glm::vec3(0.0f, 0.50980392f, 0.50980392f);
        material.specular = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
        material.shininess = 32.0f;
    }
    else if (name == "green plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.1f, 0.35f, 0.1f);
        material.specular = glm::vec3(0.45f, 0.55f, 0.45f);
        material.shininess = 32.0f;
    }
    else if (name == "red plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.5f, 0.0f, 0.0f);
        material.specular = glm::vec3(0.7f, 0.6f, 0.6f);
        material.shininess = 32.0f;
    }
    else if (name == "white plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.55f, 0.55f, 0.55f);
        material.specular = glm::vec3(0.70f, 0.70f, 0.70f);
        material.shininess = 32.0f;
    }
    else if (name == "yellow plastic")
    {
        material.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.5f, 0.5f, 0.0f);
        material.specular = glm::vec3(0.60f, 0.60f, 0.50f);
        material.shininess = 32.0f;
    }
    else if (name == "black rubber")
    {
        material.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
        material.diffuse = glm::vec3(0.01f, 0.01f, 0.01f);
        material.specular = glm::vec3(0.4f, 0.4f, 0.4f);
        material.shininess = 10.0f;
    }
    else if (name == "cyan rubber")
    {
        material.ambient = glm::vec3(0.0f, 0.05f, 0.05f);
        material.diffuse = glm::vec3(0.4f, 0.5f, 0.5f);
        material.specular = glm::vec3(0.04f, 0.7f, 0.7f);
        material.shininess = 10.0f;
    }
    else if (name == "green rubber")
    {
        material.ambient = glm::vec3(0.0f, 0.05f, 0.0f);
        material.diffuse = glm::vec3(0.4f, 0.5f, 0.4f);
        material.specular = glm::vec3(0.04f, 0.7f, 0.04f);
        material.shininess = 10.0f;
    }
    else if (name == "red rubber")
    {
        material.ambient = glm::vec3(0.05f, 0.0f, 0.0f);
        material.diffuse = glm::vec3(0.5f, 0.4f, 0.4f);
        material.specular = glm::vec3(0.7f, 0.04f, 0.04f);
        material.shininess = 10.0f;
    }
    else if (name == "white rubber")
    {
        material.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
        material.specular = glm::vec3(0.7f, 0.7f, 0.7f);
        material.shininess = 10.0f;
    }
    else if (name == "yellow rubber")
    {
        material.ambient = glm::vec3(0.05f, 0.05f, 0.0f);
        material.diffuse = glm::vec3(0.5f, 0.5f, 0.4f);
        material.specular = glm::vec3(0.7f, 0.7f, 0.04f);
        material.shininess = 10.0f;
    }

    // comment this line if you use one RGB triplet to represent light color/intensity
    material.ambient = material.diffuse;
}

hzgl::Material hzgl::CreatesSampleMaterial(MaterialType type, const std::string &name)
{
    Material material(type);

    if (type == HZGL_PHONG_MATERIAL)
        hzglFillPhongMaterial(material, name);

    return material;
}

void hzgl::SetupMaterial(GLuint program, const Material &material, std::string uName)
{
    glUseProgram(program);

    if (material.type == HZGL_PHONG_MATERIAL)
    {
        glUniform3fv(hzglLoc(program, uName + ".ambient"), 1, &material.ambient[0]);
        glUniform3fv(hzglLoc(program, uName + ".diffuse"), 1, &material.diffuse[0]);
        glUniform3fv(hzglLoc(program, uName + ".specular"), 1, &material.specular[0]);
        glUniform1f(hzglLoc(program, uName + ".shininess"), material.shininess);
    }
    else if (material.type == HZGL_PBR_MATERIAL)
    {
        glUniform3fv(hzglLoc(program, uName + ".albedo"), 1, &material.albedo[0]);
        glUniform1f(hzglLoc(program, uName + ".metallic"), material.metallic);
        glUniform1f(hzglLoc(program, uName + ".roughness"), material.roughness);
        glUniform1f(hzglLoc(program, uName + ".ao"), material.ao);
    }
}

void hzgl::SetupMaterialInArray(GLuint program, const Material& material, std::string uArrayName, int index) 
{
    std::string uName = uArrayName + "[" + std::to_string(index) + "]";
    SetupMaterial(program, material, uName);
}