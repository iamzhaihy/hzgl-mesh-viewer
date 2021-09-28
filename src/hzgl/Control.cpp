#include "Control.hpp"

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>

// disable MSVC warnings
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static std::string hzglScreenshotStem()
{
    const std::time_t now = std::time(nullptr);

    const std::tm *now_tm = std::localtime(&now);

    int year = now_tm->tm_year + 1900;
    int month = now_tm->tm_mon + 1;
    int day = now_tm->tm_mday;
    int hour = now_tm->tm_hour;
    int minute = now_tm->tm_min;
    int second = now_tm->tm_sec;

    std::stringstream timestamp;

    timestamp << "Screenshot"
              << "-" << year << "-" << month << "-" << day 
              << "-" << hour << "-" << minute << "-" << second;

    return timestamp.str();
}

void hzgl::TakeScreenshot(int x, int y, int w, int h)
{
    glReadBuffer(GL_FRONT);

    GLubyte *pixels = new GLubyte[4 * w * h];
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    std::string stem = hzglScreenshotStem();

    stbi_flip_vertically_on_write(1);
    stbi_write_png((stem + ".png").c_str(), w, h, 4, pixels, 4 * w);

    delete[] pixels;
}

void hzgl::ImGuiControl::setStyleOptions()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();

    // Color Theme: Light
    ImGui::StyleColorsLight();

    // Padding & Spacing & Alignment
    style.ItemSpacing = ImVec2(6, 6);

    style.FrameRounding = 2;
    style.FramePadding = ImVec2(4, 2);

    style.GrabMinSize = 5;
    style.GrabRounding = 2;

    style.WindowBorderSize = 0;
    style.WindowPadding = ImVec2(10, 10);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    // Color related options
    style.Colors[ImGuiCol_Header] = ImVec4(0.61f, 0.79f, 1.00f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.73f, 1.00f, 0.31f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.63f, 0.96f, 0.31f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.74f, 0.77f, 0.81f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.62f, 0.67f, 0.73f, 0.40f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.62f, 0.67f, 0.73f, 0.40f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.50f, 0.72f, 0.99f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.50f, 0.72f, 0.99f, 0.40f);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

// ref: https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp#L190
void hzgl::ImGuiControl::helpMarker(const char *desc, bool sameLine)
{
    if (sameLine) ImGui::SameLine();

    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

hzgl::ImGuiControl::ImGuiControl()
{
}

hzgl::ImGuiControl::~ImGuiControl()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void hzgl::ImGuiControl::Init(GLFWwindow *window, const char *glslVersion)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    setStyleOptions();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void hzgl::ImGuiControl::BeginFrame(bool fixed_position)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags windowFlags = 0;

    if (fixed_position)
    {
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoDecoration;

        // For now, make the control panel occupy the rightmost 1/4
        ImGuiViewport *vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + 0.75f * vp->WorkSize.x, vp->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(0.25f * vp->WorkSize.x, vp->WorkSize.y), ImGuiCond_Always);
    }
    else
    {
        ImGuiViewport *vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x, vp->WorkPos.y), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(0.3f * vp->WorkSize.x, vp->WorkSize.y), ImGuiCond_Always);
    }

    ImGui::Begin("Tools", nullptr, windowFlags);
}

void hzgl::ImGuiControl::EndFrame()
{
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void hzgl::ImGuiControl::RenderLightInfoWidget(Light& light)
{
    std::string btnText = light.isEnabled ? "Disable" : "Enable";
    btnText += " this light##light-onoff";

    // hide everything if disabled
    if (light.isEnabled)
    {
        ImGui::Text("Light type: %s", LightTypeName(light.type).c_str());
        
        std::string labelText = (light.type == HZGL_DIRECTIONAL_LIGHT)
                              ? "direction##light-direction"
                              : "position##light-position";

        // light position / direction
        ImGui::DragFloat3(labelText.c_str(), &light.position[0], 0.01f);

        // light intensities
        ImGui::Text("%s", "Color/Intensities");
        ImGui::ColorEdit3("color##light-color", &light.color[0]);
        helpMarker("the diffuse and specular components are usually the same");
        ImGui::ColorEdit3("ambient##light-ambient", &light.ambient[0]);

        if (light.type == HZGL_POINT_LIGHT || light.type == HZGL_SPOT_LIGHT) {
            ImGui::Text("%s", "Attenuation Factors");
            ImGui::DragFloat("constant##light-kconstant", &light.constantAttenuation, 0.01f);
            ImGui::DragFloat("linear##light-klinear", &light.linearAttenuation, 0.01f);
            ImGui::DragFloat("quadratic##light-kquadratic", &light.quadraticAttenuation, 0.01f);
        }
        
        if (light.type == HZGL_SPOT_LIGHT)
        {
            ImGui::Text("%s", "Cone (Spot light only)");

            ImGui::Text("%s", "Direction");
            ImGui::DragFloat3("###light-cone-dir", &light.coneDirection[0], 0.01f);

            ImGui::Text("%s", "Exponent");
            ImGui::DragFloat("###light-cone-exp", &light.spotExponent, 0.1f);

            ImGui::Text("%s", "Cosine Cutoff");
            ImGui::DragFloat("###light-cone-cos-cutoff", &light.spotCosCutoff, 0.01f, 0.0f, 1.0f);
        }
    }

    // a button to enable/disable the current light
    ImGui::Spacing();
    if (ImGui::Button(btnText.c_str(), ImVec2(-1, 0)))
        light.isEnabled = !light.isEnabled;
    ImGui::Spacing();
}

void hzgl::ImGuiControl::RenderLightingConfigWidget(std::vector<Light>& lights, int* lIndex, LightType filterType, bool collapsingHeader)
{
    if (lights.empty())
        return;
    
    static int selected = 0;
    bool useFilter = filterType != HZGL_ANY_LIGHT;

    std::vector<int> mapping;
    std::vector<std::string> lightNames;
    for (int i = 0; i < lights.size(); i++) 
    {
        if (!useFilter || lights[i].type == filterType) {
            std::string lName = "#" + std::to_string(i+1) + ": " + LightTypeName(lights[i].type);
            lightNames.push_back(lName);
            mapping.push_back(i);
        }
    }

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    
    if (ImGui::TreeNodeEx("Lighting", flags))
    {
        if (lightNames.size() > 1)
            RenderListBox("Available Lights", lightNames, &selected);

        RenderLightInfoWidget(lights[mapping[selected]]);
        *lIndex = mapping[selected];

        if (!collapsingHeader)
            ImGui::TreePop();
    }
} 

void hzgl::ImGuiControl::RenderMaterialInfoWidget(Material& material)
{
    ImGui::Text("Material type: %s", MaterialTypeName(material.type).c_str());
    
    if (material.type == MaterialType::HZGL_PHONG_MATERIAL)
    {
        ImGui::ColorEdit3("ambient##mat-phong", &material.ambient[0]);
        ImGui::ColorEdit3("diffuse##mat-phong", &material.diffuse[0]);
        ImGui::ColorEdit3("specular##mat-phong", &material.specular[0]);
        ImGui::DragFloat("shininess##mat-phong", &material.shininess, 0.1f, 2.0f, 3200.0f, "%.1f");
    }
    else if (material.type == MaterialType::HZGL_PBR_MATERIAL)
    {
        ImGui::ColorEdit3("albedo##mat-pbr", &material.albedo[0]);
        ImGui::DragFloat("metallic##mat-pbr", &material.metallic, 0.01f, 0.0f, 1.0f, "%.2f");
        ImGui::DragFloat("roughness##mat-pbr", &material.roughness, 0.01f, 0.0f, 1.0f, "%.2f");
        ImGui::DragFloat("ao##mat-pbr", &material.ao, 0.01f, 0.0f, 1.0f, "%.2f");
    }
}

void hzgl::ImGuiControl::RenderMaterialConfigWidget(std::vector<Material>& materials, int* mIndex, MaterialType filterType, bool collapsingHeader)
{
    if (materials.empty())
        return;

    static int selected = 0;
    bool useFilter = filterType != HZGL_ANY_MATERIAL;

    std::vector<int> mapping;
    std::vector<std::string> materialNames;
    for (int i = 0; i < materials.size(); i++) 
    {
        if (!useFilter || materials[i].type == filterType) {
            std::string mName = "#" + std::to_string(i+1) + ": " + MaterialTypeName(materials[i].type);
            materialNames.push_back(mName);
            mapping.push_back(i);
        }
    }

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    if (ImGui::TreeNodeEx("Materials", flags))
    {
        if (materialNames.size() > 1)
            RenderListBox("Available Materials", materialNames, &selected);

        RenderMaterialInfoWidget(materials[mapping[selected]]);
        *mIndex = mapping[selected];
        
        if (!collapsingHeader)
            ImGui::TreePop();
    }
}

void hzgl::ImGuiControl::RenderCameraWidget(Camera& camera)
{
    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::CollapsingHeader("Camera", flags))
    {
        // set camera position ("eye" for glm::lookAt())
        ImGui::Text("Position");
        helpMarker("\"eye\" for glm::lookAt()");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##position-dragf3", &camera.position[0], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::Spacing();

        // set camera target ("center" for glm::lookAt())
        ImGui::Text("Target");
        helpMarker("\"center\" for glm::lookAt()");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##target-dragf3", &camera.target[0], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::Spacing();

        // set camera vertical FoV
        ImGui::Text("Vertical FoV");
        helpMarker("[0, 180] degrees");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat("##vfov-dragf", &camera.vfov, 0.01f, 0.0f, 180.0f, "%.2f");
        ImGui::Spacing();

        // a button to reset camera properties
        if (ImGui::Button("Reset Camera Properties##camera-reset", ImVec2(-1, 0)))
        {
            camera.position = glm::vec3(0, 0, 3);
            camera.target = glm::vec3(0, 0, 0);
            camera.vfov = 45.0f;
        }

        ImGui::Spacing();
    }
}

void hzgl::ImGuiControl::RenderModelInfoWidget(const RenderObject& robj)
{
    for (int i = 0; i < robj.num_shapes; i++)
    {
        const auto& rshape = robj.shapes[i];
        std::string label = "Mesh " + std::to_string(i+1)
                          + ": " + rshape.name;

        if (ImGui::TreeNodeEx(label.c_str()))
        {
            ImGui::Text("OpenGL VAO: %u", rshape.VAO);

            if (ImGui::TreeNodeEx("Geometry"))
            {
                ImGui::Text("Number of vertices: %d", rshape.num_vertices);
                ImGui::Text("Surface normals: %s", rshape.has_normals ? "Yes" : "No");
                ImGui::Text("Texture coordinates: %s", rshape.has_texcoords ? "Yes" : "No");
                ImGui::TreePop();
            }

            if (rshape.has_textures && ImGui::TreeNodeEx("Textures"))
            {
                for (const auto & pair : rshape.texture)
                {
                    if (pair.second == 0)
                        continue;

                    ImGui::Text("%s: %u", pair.first.c_str(), pair.second);
                }
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    ImGui::Spacing();
}


void hzgl::ImGuiControl::RenderModelConfigWidget(std::vector<RenderObject>& objects, int* oIndex, bool collapsingHeader) {
    if (objects.empty())
        return;

    static int selected = 0;

    std::vector<std::string> objectPaths;
    for (int i = 0; i < objects.size(); i++)
        objectPaths.push_back(objects[i].path);

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    if (ImGui::TreeNodeEx("Assets", flags))
    {
        if (objects.size() > 1)
            RenderListBox("Available Models", objectPaths, &selected);

        RenderModelInfoWidget(objects[selected]);
        *oIndex = selected;
        
        if (!collapsingHeader)
            ImGui::TreePop();
    }
}

void hzgl::ImGuiControl::RenderShaderProgramInfoWidget(ProgramInfo& program) {
    ImGui::Text("OpenGL ID: %d", program.id);
    
    if (ImGui::TreeNodeEx("Shaders Attached"))
    {
        for (const auto& stage : program.stages)
            ImGui::Text("%s", stage.filepath.c_str());
        ImGui::TreePop();
    }
    
    ImGui::Spacing();
}

void hzgl::ImGuiControl::RenderShaderProgramConfigWidget(std::vector<ProgramInfo>& programs, int* pIndex, bool collapsingHeader) {
    if (programs.empty())
        return;

    static int selected = 0;

    std::vector<std::string> programNames;
    for (int i = 0; i < programs.size(); i++)
        programNames.push_back(programs[i].name);

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    if (ImGui::TreeNodeEx("Rendering", flags))
    {
        if (programs.size() > 1)
            RenderListBox("Available Programs", programNames, &selected);

        RenderShaderProgramInfoWidget(programs[selected]);
        *pIndex = selected;
        
        if (!collapsingHeader)
            ImGui::TreePop();
    }
}

void hzgl::ImGuiControl::RenderDragMatrix3(const std::string& label, std::vector<float>& mat)
{
    if (label.empty() || mat.size() != 9)
        return;

    std::string identifier = "##" + label + "-drag-mat3";

    ImGui::Text("%s", label.c_str());
    helpMarker("shwon in row major order");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        ImGui::DragFloat3((identifier+"-row1").c_str(), &mat[0], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3((identifier+"-row2").c_str(), &mat[3], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat3((identifier+"-row3").c_str(), &mat[6], 0.01f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
}

void hzgl::ImGuiControl::RenderListBox(const std::string &label, const std::vector<std::string> &options, int *selected)
{
    if (options.empty())
        return;

    std::string identifier = "##" + label + "-listbox";
    float listBoxHeight = ImGui::GetTextLineHeightWithSpacing() * (options.size() + 2);

    if (!label.empty())
        ImGui::Text("%s", label.c_str());
    
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginListBox(identifier.c_str(), ImVec2(0, listBoxHeight)))
    {
        for (int i = 0; i < options.size(); i++)
        {
            if (ImGui::Selectable((options[i] + identifier).c_str(), (selected && *selected == i)))
            {
                if (selected != nullptr)
                    *selected = i;
            }
        }
        ImGui::EndListBox();
    }
}