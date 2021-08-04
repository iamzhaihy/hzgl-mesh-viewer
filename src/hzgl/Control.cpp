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

static std::string hzglLightType(hzgl::LightType type)
{
    std::string lightType;

    switch (type)
    {
        case hzgl::LightType::HZGL_SPOT_LIGHT:
            lightType = "Spot light";
            break;
        case hzgl::LightType::HZGL_POINT_LIGHT:
            lightType = "Point light";
            break;
        case hzgl::LightType::HZGL_DIRECTIONAL_LIGHT:
            lightType = "Directional light";
            break;
        default:
            lightType = "Unknown type";
            break;
    }

    return lightType;
}

static std::string hzglMaterialType(hzgl::MaterialType type)
{
    std::string matType;

    switch (type)
    {
        case hzgl::MaterialType::HZGL_PHONG_MATERIAL:
            matType = "Phong Material";
            break;
        case hzgl::MaterialType::HZGL_PBR_MATERIAL:
            matType = "PBR Material";
            break;
        default:
            matType = "Unknown type";
            break;
    }

    return matType;
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
void hzgl::ImGuiControl::helpMarker(const char *desc)
{
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

void hzgl::ImGuiControl::RenderLightWidget(Light& light) 
{
    std::string btnText = light.isEnabled ? "Disable" : "Enable";
    btnText += " this light";

    // light position / direction
    if (light.type == HZGL_DIRECTIONAL_LIGHT)
        ImGui::Text("%s", "Direction");
    else
        ImGui::Text("%s", "Position");

    ImGui::DragFloat3("###light-position-drag3", &light.position[0], 0.01f);

    // light intensities
    ImGui::Text("%s", "Color/Intensities");
    ImGui::ColorEdit3("color", &light.color[0]); ImGui::SameLine(); 
    helpMarker("the diffuse and specular components are usually the same");
    ImGui::ColorEdit3("ambient", &light.ambient[0]);

    if (light.type == HZGL_SPOT_LIGHT)
    {
        ImGui::Text("%s", "Cone (Spot light only)");

        ImGui::Text("%s", "Direction");
        ImGui::DragFloat3("###light-cone-dir-drag3", &light.coneDirection[0], 0.01f);

        ImGui::Text("%s", "Exponent");
        ImGui::DragFloat("###light-cone-exp", &light.spotExponent, 0.1f);

        ImGui::Text("%s", "Cosine Cutoff");
        ImGui::DragFloat("###light-cone-cos-cutoff", &light.spotCosCutoff, 0.01f, 0.0f, 1.0f);
    }
    else {
        // TODO: Handle attenuation in shader
        // ImGui::Text("%s", "Attenuation Factors");
        // ImGui::DragFloat("constant", &light.constantAttenuation, 0.01f);
        // ImGui::DragFloat("linear", &light.linearAttenuation, 0.01f);
        // ImGui::DragFloat("quadratic", &light.quadraticAttenuation, 0.01f);
    }

    if (ImGui::Button(btnText.c_str(), ImVec2(-1, 0)))
        light.isEnabled = !light.isEnabled;
}

void hzgl::ImGuiControl::RenderLightingConfigWidget(std::vector<Light>& lights, bool collapsingHeader)
{
    if (lights.empty())
        return;

    int lIndex = 0;

    std::vector<std::string> lightNames;
    for (int i = 0; i < lights.size(); i++) 
    {
        std::string lName = "#" + std::to_string(i+1) + ": " + hzglLightType(lights[i].type);
        lightNames.push_back(lName);
    }

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    if (ImGui::TreeNodeEx("Lighting Properties", flags))
    {
        if (lightNames.size() > 1)
            RenderListBox("", lightNames, &lIndex);

        RenderLightWidget(lights[lIndex]);

        if (!collapsingHeader)
            ImGui::TreePop();
    }
} 

void hzgl::ImGuiControl::RenderMaterialWidget(Material& material)
{
    ImGui::ColorEdit3("ambient", &material.ambient[0]);
    ImGui::ColorEdit3("diffuse", &material.diffuse[0]);
    ImGui::ColorEdit3("specular", &material.specular[0]);
    ImGui::DragFloat("shininess", &material.shininess, 0.1f);
}

void hzgl::ImGuiControl::RenderMaterialConfigWidget(std::vector<Material>& materials, bool collapsingHeader)
{
    if (materials.empty())
        return;

    int mIndex = 0;

    std::vector<std::string> materialNames;
    for (int i = 0; i < materials.size(); i++) 
    {
        std::string mName = "#" + std::to_string(i+1) + ": " + hzglMaterialType(materials[i].type);
        materialNames.push_back(mName);
    }

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if (collapsingHeader)
        flags |= ImGuiTreeNodeFlags_CollapsingHeader;

    if (ImGui::TreeNodeEx("Material Properties", flags))
    {
        if (materialNames.size() > 1)
            RenderListBox("", materialNames, &mIndex);

        RenderMaterialWidget(materials[mIndex]);

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
        ImGui::SameLine();
        helpMarker("\"eye\" for glm::lookAt()");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##position-dragf3", &camera.position[0], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::Spacing();

        // set camera target ("center" for glm::lookAt())
        ImGui::Text("Target");
        ImGui::SameLine();
        helpMarker("\"center\" for glm::lookAt()");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##target-dragf3", &camera.target[0], 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::Spacing();

        // set camera vertical FoV
        ImGui::Text("Vertical FoV");
        ImGui::SameLine();
        helpMarker("[0, 180] degrees");

        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat("##vfov-dragf", &camera.vfov, 0.01f, 0.0f, 180.0f, "%.2f");
        ImGui::Spacing();

        // a button to reset camera properties
        if (ImGui::Button("Reset Camera Properties", ImVec2(-1, 0)))
        {
            camera.position = glm::vec3(0, 0, 3);
            camera.target = glm::vec3(0, 0, 0);
            camera.vfov = 45.0f;
        }

        ImGui::Spacing();
    }
}

void hzgl::ImGuiControl::RenderDragMatrix3(const std::string& label, std::vector<float>& mat)
{
    if (label.empty() || mat.size() != 9)
        return;

    std::string identifier = "##" + label + "-drag-mat3";

    ImGui::Text("%s", label.c_str());
    ImGui::SameLine(); helpMarker("shwon in row major order");

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
            if (ImGui::Selectable(options[i].c_str(), (selected && *selected == i)))
            {
                if (selected != nullptr)
                    *selected = i;
            }
        }
        ImGui::EndListBox();
    }
}