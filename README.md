# OpenGL Mesh Viewer

Hongyu ZHAI ([Github](https://github.com/iamzhaihy), [LinkedIn](https://www.linkedin.com/in/hongyu-zhai-34b961139/))

![Main UI](./results/main-ui.png)

## About this Project
When I was learning to program shaders, I almost always spent more time on setting up the environment. OpenGL requires a lot of boilerplate code to work, and some third-party libraries are necessary to do the heavy lifting. To enable faster shader development, I wrote this project. It does the preparation for you, so the user can focus on implementing exciting rendering algorithms.

## Compile and Run
Before you start, make sure you have the following things:
- CMake 3.15+
- A C++ compiler for your platform
- Hardware that supports modern OpenGL

To run the program yourself, type the following commands
```bash
git clone --recursive https://github.com/iamzhaihy/hzgl-obj-viewer.git
cd hzgl-obj-viewer
mkdir build && cd build
cmake ..
cmake --build .
```

The external libraries are all compiled statically, which means it should work out of the box if you have the things above.

## Basic Controls

Using the GUI, the user can:

- Tweak camera settings by clicking and dragging the widgets in the "Camera" section
- Switch between available meshes by clicking on an item in the "Loaded Meshes" list
- Switch between shader programs by clicking on an item in the "Shader Programs" list
  - Tweak additional rendering settings (if available) via the widgets

Several keyboard shortcuts are provided:

- Press `Q` or `Escape` to quit the program
- Press `F` or `PrintScreen` to take a screenshot (will be stored in the same directory as the executable)

## Current Features

**Easy-to-use helper functions**

During the process, I wrote several helper functions to make things easier. For example:
```c++
// src/hzgl/FileSystem.hpp
bool Exists(const std::string& filename);
```

```c++
// src/hzgl/Mesh.hpp
typedef struct
{
    // Metadata
    std::string name;

    // Geometry
    int num_vertices;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texcoords;

    // Material
    ShadingMode shading_mode;
    std::unordered_map<std::string, std::string> texpath;
} MeshInfo;

void LoadOBJ(const std::string& filepath, std::vector<MeshInfo>& meshes);
```

```c++
typedef struct
{
    GLenum type;
    std::string filepath;
    GLuint id;
} ShaderStage;

GLuint CreateShaderProgram(std::vector<ShaderStage> stages);
```

I also added several helper classes. One of them is `ResourceManager`. It will check to make sure the same file does not get loaded twice. It will also release the resources when it is out of scope. For more detail please check out `/src/hzgl/ResourceManager.hpp`.

```c++
hzgl::ResourceManager resources;
std::vector<hzgl::RenderObject> objects;

// load meshes from OBJ files
resources.LoadMesh("../assets/models/bunny.obj", objects);

// load and create shader program from source files
resources.LoadShaderProgram({
    {GL_VERTEX_SHADER, "../assets/shaders/phong.vert"},
    {GL_FRAGMENT_SHADER, "../assets/shaders/phong.frag"},
    }, "Blinn-Phong Shading");
```

**Interactive UI for Faster Debugging**

This project incorporated [Dear ImGui](https://github.com/ocornut/imgui) to make debugging easier. The user can tweak the camera properties using the tools in the first section.

![gui camera](./results/gui-camera.gif)

As mentioned before, it is easy to switch between loaded OBJ files. When developing shaders, this can be very helpful. For example, If the shader works on the first mesh but does not work on the second, then the surface normals of the second mesh might be incorrect.

![gui mesh select](./results/gui-mesh-select.gif)

It is also easy to switch between shader programs. Depending on the selected shader program, Additional options may pop up. 

![gui shader select](./results/gui-shader-select.gif)

As shown above, when doing Blinn-Phong Shading, it is helpful to see how the lighting affects the final result.



## Future Plans for the Project

Here is my plan for the future improvement

- More sample meshes
  - Add more OBJ files from [McGuire Computer Graphics Archive](http://casual-effects.com/data/index.html)
- More rendering methods
  - Bump, Normal, and Parallax mapping 
  - Physically Based Rendering
- Improve mesh loading code
  - Handle OBJ files with more than 1 shape
  - Handle OBJ files with smoothing groups
  - Handle OBJ files with missing info (missing normals, TBN, etc.)
  - A better way to determine texture types (OBJ was made before PBR and 3D artists are not consistent)
- Improve interactive UI
  - Load OBJ file from a dropdown file menu
  - Add a simple editor that recompiles shaders after the user saves changes
- Improve helper functions
  - Make `hzgl` a standalone library
    - Make it easier to incorporate
  - More functionality in `FileSystem.hpp`
    - Add fallback options if C++ 17 is not supported
  -  Support more file formats (add [tinygltf](https://github.com/syoyo/tinygltf) or switch to [ASSIMP](https://github.com/assimp/assimp))
