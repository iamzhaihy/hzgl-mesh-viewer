#pragma once

#include "Light.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "ResourceManager.hpp"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace hzgl
{
    void TakeScreenshot(int x, int y, int w, int h);

    class ImGuiControl {
    private:
        bool _isActive;

        // set up style for initial use
        void setStyleOptions();

        // render control widgets
        void helpMarker(const char* desc, bool sameLine = true);

    public:
        ImGuiControl();
        ~ImGuiControl();

        void Init(GLFWwindow* window, const char* glslVersion);

        void BeginFrame(bool fixed_position = true);
        void EndFrame();

        // widget to control hzgl types
        void RenderCameraWidget(Camera& camera);

        void RenderModelInfoWidget(const RenderObject& robj);
        void RenderModelConfigWidget(std::vector<RenderObject>& objects, int* oIndex, bool collapsingHeader = true);

        void RenderLightInfoWidget(Light& light);
        void RenderLightingConfigWidget(std::vector<Light>& lights, int* lIndex, LightType filterType = HZGL_ANY_LIGHT, bool collapsingHeader = true);

        void RenderMaterialInfoWidget(Material& material);
        void RenderMaterialConfigWidget(std::vector<Material>& materials, int* mIndex, MaterialType filterType = HZGL_ANY_MATERIAL, bool collapsingHeader = true);
        
        void RenderShaderProgramInfoWidget(ProgramInfo& program);
        void RenderShaderProgramConfigWidget(std::vector<ProgramInfo>& programs, int* pIndex, bool collapsingHeader = true);

        // wrapper around Dear ImGui
        void RenderDragMatrix3(const std::string& label, std::vector<float>& mat);
        void RenderListBox(const std::string& label, const std::vector<std::string>& options, int* selected = nullptr);
    };
} // namespace hzgl
