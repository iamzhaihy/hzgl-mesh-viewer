#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#if defined(DEBUG) || defined(_DEBUG)
#define GLM_FORCE_MESSAGES
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// Helper functions
#include "hzgl/Timer.hpp"
#include "hzgl/Light.hpp"
#include "hzgl/Material.hpp"
#include "hzgl/Camera.hpp"
#include "hzgl/Control.hpp"
#include "hzgl/ResourceManager.hpp"

static int width = 1280;
static int height = 720;
static float deltaTime = 0.0f;

GLFWwindow* window;

hzgl::SimpleTimer timer;
hzgl::ImGuiControl guiControl;
hzgl::ResourceManager resources;
std::vector<hzgl::Light> lights;
std::vector<hzgl::Material> materials;
std::vector<hzgl::RenderObject> objects;
hzgl::Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 45.0f,
    static_cast<float>(0.75f * width) / static_cast<float>(height));

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void init(void)
{
    // load meshes from OBJ files
    resources.LoadMesh("../assets/models/bunny.obj", objects);
    resources.LoadMesh("../assets/models/buddha.obj", objects);
    resources.LoadMesh("../assets/models/dragon.obj", objects);

    // prepare the shader programs
    resources.LoadShaderProgram({
        {GL_VERTEX_SHADER, "../assets/shaders/passthrough.vert"},
        {GL_FRAGMENT_SHADER, "../assets/shaders/passthrough.frag"},
        }, "Rendering Normal");

    resources.LoadShaderProgram({
        {GL_VERTEX_SHADER, "../assets/shaders/phong.vert"},
        {GL_FRAGMENT_SHADER, "../assets/shaders/phong.frag"},
        }, "Blinn-Phong Shading");

    guiControl.Init(window, "#version 410");

    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);

    lights.push_back(
        hzgl::Light(hzgl::LightType::HZGL_POINT_LIGHT, camera.position, glm::vec3(1, 1, 1), glm::vec3(.1, .1, .1)));

    materials.push_back(
        hzgl::Material(hzgl::MaterialType::HZGL_PHONG_MATERIAL, glm::vec3(0.54f, 0.54f, 0.86f), glm::vec3(0.54f, 0.54f, 0.86f), glm::vec3(0.2f, 0.2f, 0.2f)));

    glViewport(0, 0, static_cast<int>(0.75f * width), height);
    glClearColor(0.98f, 0.98f, 0.98f, 1.0f);

    // enable optional functionalities
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

static void display(void)
{
    static int mIndex = 0;
    static int pIndex = 0;
    static float rotation = 0.0f;

    deltaTime = static_cast<float>(timer.Tick());
    rotation += 10.0f * deltaTime;
    if (rotation > 360.0f) rotation -= 360.0f;

    const auto modelNames = resources.GetLoadedMeshesNames();
    const auto programNames = resources.GetLoadedShaderProgramNames();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    guiControl.BeginFrame(true);
    {	
        guiControl.RenderCameraWidget(camera);

        if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen))
            guiControl.RenderListBox("Loaded Meshes", modelNames, &mIndex);

        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
        {
            guiControl.RenderListBox("Shader Programs", programNames, &pIndex);

            if (programNames[pIndex] == "Blinn-Phong Shading")
            {
                guiControl.RenderLightingConfigWidget(lights, false);
                guiControl.RenderMaterialConfigWidget(materials, false);
            }
        }
    }
    guiControl.EndFrame();

    GLuint program = resources.GetProgramID(pIndex);

    glUseProgram(program);

    glm::mat4 Model = glm::rotate(glm::radians(rotation), glm::vec3(0, 1, 0));
    glm::mat4 View = camera.GetViewMatrix();
    glm::mat4 Projection = camera.GetProjMatrix();
    glm::mat4 Normal = glm::transpose(glm::inverse(Model));

    glUniformMatrix4fv(glGetUniformLocation(program, "Model"), 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "View"), 1, GL_FALSE, &View[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "Projection"), 1, GL_FALSE, &Projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "Normal"), 1, GL_FALSE, &Normal[0][0]);

    if (programNames[pIndex] == "Blinn-Phong Shading")
    {
        hzgl::SetupLight(program, lights[0], "uLight");
        hzgl::SetupMaterial(program, materials[0], "uMaterial");
        glUniform3fv(glGetUniformLocation(program, "uEyePosition"), 1, &camera.position[0]);
    }

    glBindVertexArray(objects[mIndex].VAO);
    glDrawArrays(GL_TRIANGLES, 0, objects[mIndex].num_vertices);

    glBindVertexArray(0);
    glUseProgram(0);
}

int main(void)
{
    // initialize GLFW
    if (!glfwInit())
        return -1;

    // use OpenGL 4.1 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // use 8x multi-sampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, "OBJ Viewer", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    // enable vertical sync
    glfwSwapInterval(1);

    // register callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load GL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        display();

        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0)
        return;

    glViewport(0, 0, static_cast<int>(0.75f * width), height);
    camera.aspect_ratio = static_cast<float>(0.75f * width) / static_cast<float>(height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_F:
        case GLFW_KEY_PRINT_SCREEN:
            hzgl::TakeScreenshot(0, 0, width, height);
            break;
        default:
            break;
        }
    }
}