// BGFX_PROTOTYPE.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "InputManager.h"
#include "Camera.h"


#include "BGFX_PROTOTYPE.h"
#include <bgfx/bgfx.h>
#include <bx/uint32_t.h>
#include <bgfx/platform.h>
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>



#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
#include "ObjLoader.h" // Add this new include

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

static bool s_showStats = false;

static void glfw_errorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

bgfx::ShaderHandle loadShader(const char* shaderPath)
{
    std::ifstream file(shaderPath, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to load shader: " << shaderPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(static_cast<size_t>(fileSize));
    file.read(buffer.data(), fileSize);

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), static_cast<uint32_t>(fileSize));
    std::cout << "Shader loaded: " << shaderPath << std::endl;
    return bgfx::createShader(mem);
}

int main(void)
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "BGFX_PROTOTYPE", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, glfw_keyCallback);
    InputManager::initialize(window);

    // Initialize BGFX
    bgfx::renderFrame();

    bgfx::Init bgfxinit;
    bgfxinit.type = bgfx::RendererType::OpenGL;
    bgfxinit.resolution.width = WNDW_WIDTH;
    bgfxinit.resolution.height = WNDW_HEIGHT;
    bgfxinit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxinit.platformData.nwh = glfwGetWin32Window(window);
    if (!bgfx::init(bgfxinit)) {
        std::cerr << "Failed to initialize BGFX" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Enable debug output
    bgfx::setDebug(BGFX_DEBUG_TEXT); // <-- Add this line here

    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);


    // Load shaders
    bgfx::ShaderHandle vsh = loadShader("C:\\Users\\mario\\source\\repos\\BGFX_PROTOTYPE\\vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("C:\\Users\\mario\\source\\repos\\BGFX_PROTOTYPE\\fs_cubes.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    if (!bgfx::isValid(program)) {
        std::cerr << "Failed to create shader program" << std::endl;
        return -1;
    }

    // Load OBJ file
    std::vector<ObjLoader::Vertex> vertices;
    std::vector<uint16_t> indices;
    if (!ObjLoader::loadObj("C:\\Users\\mario\\source\\repos\\BGFX_PROTOTYPE\\bunny.obj", vertices, indices)) {
        std::cerr << "Failed to load OBJ file" << std::endl;
        return -1;
    }

    bgfx::VertexBufferHandle vbh = ObjLoader::createVertexBuffer(vertices);
    bgfx::IndexBufferHandle ibh = ObjLoader::createIndexBuffer(indices);

    Camera camera;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Handle input
        InputManager::update(camera, 0.016f);

        // Begin frame
        bgfx::touch(0);

        // Set view and projection matrix
        float view[16];
        bx::mtxLookAt(view, camera.position, bx::add(camera.position, camera.front), camera.up);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set model matrix
        float mtx[16];
        bx::mtxSRT(mtx, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        bgfx::setTransform(mtx);

        // Set vertex and index buffers
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        // Submit draw call
        bgfx::submit(0, program);

        // Debug text
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/01-cubes");
        bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Rendering simple static mesh.");
        bgfx::dbgTextPrintf(0, 3, 0x0f, "Frame: % 7.3f[ms]", 1000.0f / bgfx::getStats()->cpuTimeFrame);

        // Enable stats or debug text
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

        // End frame
        bgfx::frame();
    }

    // Cleanup
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(program);

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}