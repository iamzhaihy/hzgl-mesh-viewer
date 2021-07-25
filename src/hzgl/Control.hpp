#pragma once

#include "Camera.hpp"
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

        void RenderCameraWidget(Camera* camera = nullptr);
        void RenderDragMatrix3(const std::string& label, std::vector<float>& mat);
        void RenderListBox(const std::string& label, const std::vector<std::string>& options, int* selected = nullptr); 
    };
} // namespace hzgl