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

using namespace std;

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

static bx::FileReaderI* s_fileReader = NULL;

//-----------------------------------------------------------------------

static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const bx::FilePath& _filePath)
{
    if (bx::open(_reader, _filePath))
    {
        uint32_t size = (uint32_t)bx::getSize(_reader);
        const bgfx::Memory* mem = bgfx::alloc(size + 1);
        bx::read(_reader, mem->data, size, bx::ErrorAssert{});
        bx::close(_reader);
        mem->data[mem->size - 1] = '\0';
        return mem;
    }

    //DBG("Failed to load %s.", _filePath.getCPtr());
    return NULL;
}

//static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const bx::StringView& _name)
//{
//    bx::FilePath filePath("./");
//
//    char fileName[512];
//    bx::strCopy(fileName, BX_COUNTOF(fileName), _name);
//    bx::strCat(fileName, BX_COUNTOF(fileName), ".bin");
//
//    filePath.join(fileName);
//
//    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, filePath.getCPtr()));
//    bgfx::setName(handle, _name.getPtr(), _name.getLength());
//
//    return handle;
//}
//
//bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const bx::StringView& _vsName, const bx::StringView& _fsName)
//{
//    bgfx::ShaderHandle vsh = loadShader(_reader, _vsName);
//    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
//    if (!_fsName.isEmpty())
//    {
//        fsh = loadShader(_reader, _fsName);
//    }
//
//    return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
//}
//
//bgfx::ProgramHandle loadProgram(const bx::StringView& _vsName, const bx::StringView& _fsName)
//{
//    return loadProgram(s_fileReader, _vsName, _fsName);
//}



//-----------------------------------------------------------------------

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
    cout << "shaders loaded" << endl;
    return bgfx::createShader(mem);
}

//-----------------------------------------------------------------------

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7,
};

//-----------------------------------------------------------------------


void updateCamera(Camera& camera, float deltaTime)
{
	const float cameraSpeed = 2.5f * deltaTime;
	if (InputManager::isKeyPressed(GLFW_KEY_W))
		camera.position = bx::mad(camera.front, bx::Vec3(cameraSpeed, cameraSpeed, cameraSpeed), camera.position);
	if (InputManager::isKeyPressed(GLFW_KEY_S))
		camera.position = bx::mad(camera.front, bx::Vec3(-cameraSpeed, -cameraSpeed, -cameraSpeed), camera.position);
	if (InputManager::isKeyPressed(GLFW_KEY_A))
		camera.position = bx::mad(camera.right, bx::Vec3(-cameraSpeed, -cameraSpeed, -cameraSpeed), camera.position);
	if (InputManager::isKeyPressed(GLFW_KEY_D))
		camera.position = bx::mad(camera.right, bx::Vec3(cameraSpeed, cameraSpeed, cameraSpeed), camera.position);

	double x, y;
	InputManager::getMouseMovement(&x, &y);
	const float sensitivity = 0.1f;
	camera.yaw += x * sensitivity;
	camera.pitch += y * sensitivity;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;

    bx::Vec3 direction = bx::Vec3(cos(bx::toRad(camera.yaw)) * cos(bx::toRad(camera.pitch)), sin(bx::toRad(camera.pitch)), sin(bx::toRad(camera.yaw)) * cos(bx::toRad(camera.pitch)));
	camera.front = bx::normalize(direction);

	camera.right = bx::normalize(bx::cross(camera.front, bx::Vec3(0.0f, 1.0f, 0.0f)));
	camera.up = bx::normalize(bx::cross(camera.right, camera.front));

}


//-----------------------------------------------------------------------


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

int main(void)
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "BGFX_PROTOTYPE", NULL, NULL);

    glfwSetKeyCallback(window, glfw_keyCallback);

    InputManager::initialize(window);
    
    bgfx::renderFrame();

    bgfx::Init bgfxinit;
    bgfxinit.type = bgfx::RendererType::OpenGL;
    bgfxinit.resolution.width = WNDW_WIDTH;
    bgfxinit.resolution.height = WNDW_HEIGHT;
    bgfxinit.resolution.reset = BGFX_RESET_VSYNC;
    bgfxinit.platformData.nwh = glfwGetWin32Window(window);
    bgfx::init(bgfxinit);

    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x07374ecff, 1.0f, 0);

    bgfx::ShaderHandle vsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\vs_cube.bin");
    bgfx::ShaderHandle fsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\fs_cube.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
    

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true, true)
        .end();

    bgfx::VertexBufferHandle vbh_cube = bgfx::createVertexBuffer(
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)),
        layout
    );

    bgfx::IndexBufferHandle ibh_cube = bgfx::createIndexBuffer(
        bgfx::makeRef(cubeTriList, sizeof(cubeTriList))
    );

    std::vector<ObjLoader::Vertex> vertices;
    std::vector<uint16_t> indices;
    if (!ObjLoader::loadObj("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\bunny.obj", vertices, indices)) {
        std::cerr << "Failed to load OBJ file" << std::endl;
        return -1;
    }

    bgfx::VertexBufferHandle vbh = ObjLoader::createVertexBuffer(vertices);
    bgfx::IndexBufferHandle ibh = ObjLoader::createIndexBuffer(indices);

    bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

    Camera camera;

    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();

        bgfx::touch(0);

		InputManager::update(camera, 0.016f);

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
        bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
        bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
        // Enable stats or debug text.
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

        const bx::Vec3 at = { 0.0f, 1.0f,  0.0f };
        const bx::Vec3 eye = { 0.0f, 1.0f, -2.5f };

        {
            float view[16];
            //bx::mtxLookAt(view, eye, at);
            bx::mtxLookAt(view, camera.position, bx::add(camera.position, camera.front), camera.up);


            float proj[16];
            //bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(WNDW_WIDTH), uint16_t(WNDW_HEIGHT));

        }

        float mtx[16];
        bx::mtxSRT(mtx, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.1f, -0.7f, 0.1f); // Scale and position the cube
        bgfx::setTransform(mtx);


		float timevalue = float(glfwGetTime());

		bgfx::setUniform(u_time, &timevalue);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        bgfx::setVertexBuffer(0, vbh_cube);
        bgfx::setIndexBuffer(ibh_cube);
        bgfx::submit(0, program);
        //meshSubmit(bunny, 0, program, mtx);

        


        bgfx::frame();
    }

    bgfx::shutdown();
	glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}


/*
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "BGFX_PROTOTYPE", NULL, NULL);

    bgfx::PlatformData pd;
    pd.nwh = glfwGetWin32Window(window);
    bgfx::setPlatformData(pd);

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::OpenGL; // Automatically choose a renderer.
    bgfxInit.resolution.width = WNDW_WIDTH;
    bgfxInit.resolution.height = WNDW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(bgfxInit);
*/