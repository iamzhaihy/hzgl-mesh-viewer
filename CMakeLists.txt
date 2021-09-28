cmake_minimum_required(VERSION 3.15)
project(gl-mesh-viewer C CXX)

set(CMAKE_CXX_STANDARD 17)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG}" "${CMAKE_BINARY_DIR}")
    set("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG}" "${CMAKE_BINARY_DIR}")
    set("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG}" "${CMAKE_BINARY_DIR}")
endforeach(CONFIG CMAKE_CONFIGURATION_TYPES)

if (APPLE)
    set(other_flags "${other_flags} -frtti -fvisibility-inlines-hidden")
    set(other_flags "${other_flags} -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${other_flags}")
endif(APPLE)

# Add src folder to the include directories
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Directory containing external libraries
set(EXTERN_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

# Add OpenGL to project
find_package(OpenGL REQUIRED)
set(LIBRARIES ${OPENGL_LIBRARIES})

# Include GLM for linear algebra
list(APPEND INCLUDE_DIRS "${EXTERN_LIBRARY_DIR}/glm")

# Include STB for loading images
list(APPEND INCLUDE_DIRS "${EXTERN_LIBRARY_DIR}/stb")

# Compile GLFW3 statically
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_INSTALL OFF)
option(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory("${EXTERN_LIBRARY_DIR}/glfw" "glfw")
list(APPEND INCLUDE_DIRS "${EXTERN_LIBRARY_DIR}/glfw/include")
list(APPEND LIBRARIES "glfw")

# Include GLAD to load modern OpenGL
add_subdirectory("${EXTERN_LIBRARY_DIR}/glad" "glad")
list(APPEND LIBRARIES "glad")

# Include ASSIMP to load more 3D model formats
add_subdirectory("${EXTERN_LIBRARY_DIR}/assimp" "assimp")
option(ASSIMP_NO_EXPORT ON)
option(ASSIMP_INSTALL_PDB OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
list(APPEND INCLUDE_DIRS "${EXTERN_LIBRARY_DIR}/assimp/include")
list(APPEND LIBRARIES "assimp")

# Include Dear ImGui for interactive control
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

list(APPEND INCLUDE_DIRS "${EXTERN_LIBRARY_DIR}/imgui"
                         "${EXTERN_LIBRARY_DIR}/imgui/backends")

file(GLOB PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
                          "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
                          "${CMAKE_CURRENT_SOURCE_DIR}/src/hzgl/*.cpp")
file(GLOB PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
                          "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
                          "${CMAKE_CURRENT_SOURCE_DIR}/src/hzgl/*.hpp")

list(APPEND PROJECT_SOURCES
    "${EXTERN_LIBRARY_DIR}/imgui/imgui.cpp"
    "${EXTERN_LIBRARY_DIR}/imgui/imgui_draw.cpp"
    "${EXTERN_LIBRARY_DIR}/imgui/imgui_tables.cpp"
    "${EXTERN_LIBRARY_DIR}/imgui/imgui_widgets.cpp"
    "${EXTERN_LIBRARY_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    "${EXTERN_LIBRARY_DIR}/imgui/backends/imgui_impl_opengl3.cpp")

add_executable("${PROJECT_NAME}_bin" ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories("${PROJECT_NAME}_bin" PRIVATE ${INCLUDE_DIRS})
target_link_libraries("${PROJECT_NAME}_bin" ${LIBRARIES})
