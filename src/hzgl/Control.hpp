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
        void helpMarker(const char* desc);

    public:
        ImGuiControl();
        ~ImGuiControl();

        void Init(GLFWwindow* window, const char* glslVersion);

        void BeginFrame(bool fixed_position = true);
        void EndFrame();

        // widget to control hzgl types
        void RenderCameraWidget(Camera& camera);

        void RenderMeshInfoWidget(const RenderObject& robj);

        void RenderLightWidget(Light& light);
        void RenderLightingConfigWidget(std::vector<Light>& lights, bool collapsingHeader = false);

        void RenderMaterialWidget(Material& material);
        void RenderMaterialConfigWidget(std::vector<Material>& materials, bool collapsingHeader = false);

        // wrapper around Dear ImGui
        void RenderDragMatrix3(const std::string& label, std::vector<float>& mat);
        void RenderListBox(const std::string& label, const std::vector<std::string>& options, int* selected = nullptr);
    };
} // namespace hzgl