// BGFX_PROTOTYPE.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "InputManager.h"
#include "Camera.h"
//#include "entry.h"

#include "BGFX_PROTOTYPE.h"
#include <bgfx/bgfx.h>
#include <bx/uint32_t.h>
#include <bgfx/platform.h>
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

#include <bgfx/defines.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>


#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif
#include "ObjLoader.h" // Add this new include

#define WNDW_WIDTH 1600
#define WNDW_HEIGHT 900

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

static PosColorVertex planeVertices[] =
{
    { -10.0f, -5.0f, -10.0f, 0xff888888 },  // Bottom-left
    {  10.0f, -5.0f, -10.0f, 0xff888888 },  // Bottom-right
    { -10.0f, -5.0f,  10.0f, 0xff888888 },  // Top-left
    {  10.0f, -5.0f,  10.0f, 0xff888888 },  // Top-right
};

static const uint16_t planeIndices[] =
{
    0, 1, 2,  // First triangle
    1, 3, 2   // Second triangle
};

struct Instance
{
    float position[3];
    bgfx::VertexBufferHandle vertexBuffer;
    bgfx::IndexBufferHandle indexBuffer;

    Instance(float x, float y, float z, bgfx::VertexBufferHandle vbh, bgfx::IndexBufferHandle ibh)
        : vertexBuffer(vbh), indexBuffer(ibh)
    {
        position[0] = x;
        position[1] = y;
        position[2] = z;
    }
};

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
    //std::cout << "Shader loaded: " << shaderPath << std::endl;
    return bgfx::createShader(mem);
}

bgfx::UniformHandle u_lightDir;
bgfx::UniformHandle u_lightColor;
bgfx::UniformHandle u_viewPos;

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
    //test here
    //entry::getNativeDisplayHandle();

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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    

    // Enable debug output
    bgfx::setDebug(BGFX_DEBUG_TEXT); // <-- Add this line here

    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);


    // Load shaders
    bgfx::ShaderHandle vsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\fs_cubes.bin");
    bgfx::ProgramHandle defaultProgram = bgfx::createProgram(vsh, fsh, true);

    /*if (!bgfx::isValid(program)) {
        std::cerr << "Failed to create shader program" << std::endl;
        return -1;
    }*/
    

    //cube render
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
    
    //plane render
    bgfx::VertexBufferHandle vbh_plane = bgfx::createVertexBuffer(
        bgfx::makeRef(planeVertices, sizeof(planeVertices)),
        layout
    );

    bgfx::IndexBufferHandle ibh_plane = bgfx::createIndexBuffer(
        bgfx::makeRef(planeIndices, sizeof(planeIndices))
    );

    // Load OBJ file
    std::vector<ObjLoader::Vertex> vertices;
    std::vector<uint16_t> indices;
    if (!ObjLoader::loadObj("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\suzanne.obj", vertices, indices)) {
        std::cerr << "Failed to load OBJ file" << std::endl;
        return -1;
    }

    bgfx::VertexBufferHandle vbh = ObjLoader::createVertexBuffer(vertices);
    bgfx::IndexBufferHandle ibh = ObjLoader::createIndexBuffer(indices);

    Camera camera;

    std::vector<Instance> instances;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Handle input
        InputManager::update(camera, 0.016f);

        if (InputManager::isMouseClicked(GLFW_MOUSE_BUTTON_LEFT))
        {

            float spawnDistance = 5.0f;
            // Position for new instance, e.g., random or predefined position
            float x = camera.position.x + camera.front.x * spawnDistance;
            float y = camera.position.y + camera.front.y * spawnDistance;
            float z = camera.position.z + camera.front.z * spawnDistance;

            // Create a new instance with the current vertex and index buffers
            instances.emplace_back(x, y, z, vbh, ibh);
            std::cout << "New instance created at (" << x << ", " << y << ", " << z << ")" << std::endl;
        }

        if (InputManager::isMouseClicked(GLFW_MOUSE_BUTTON_RIGHT) && !instances.empty())
        {
            instances.pop_back();
            std::cout << "Last instance deleted" << std::endl;
        }

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

        // Begin frame
        bgfx::touch(0);

        // Create uniform handles for the light direction and color
        u_lightDir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
        u_lightColor = bgfx::createUniform("u_lightColor", bgfx::UniformType::Vec4);
        u_viewPos = bgfx::createUniform("u_viewPos", bgfx::UniformType::Vec4);

        // Load the cel shading shaders instead of the current cubes shaders
        bgfx::ShaderHandle vsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\vs_cel.bin");
        bgfx::ShaderHandle fsh = loadShader("F:\\Files\\College Stuff\\programs\\Repositories\\BGFX_PROTOTYPE\\hatchfrag20.bin");
        bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

        float planeModel[16];
        bx::mtxIdentity(planeModel);
        bgfx::setTransform(planeModel);
        bgfx::setVertexBuffer(0, vbh_plane);
        bgfx::setIndexBuffer(ibh_plane);
        bgfx::submit(0, defaultProgram);

        /*for (const auto& instance : instances)
        {
            float model[16];
            bx::mtxTranslate(model, instance.position[0], instance.position[1], instance.position[2]);
            bgfx::setTransform(model);

            bgfx::setVertexBuffer(0, instance.vertexBuffer);
            bgfx::setIndexBuffer(instance.indexBuffer);
            bgfx::submit(0, program);
        }*/

        for (const auto& instance : instances)
        {
            float model[16];
            // Calculate the forward vector pointing towards the camera
            bx::Vec3 modelToCamera = bx::normalize(bx::Vec3(camera.position.x - instance.position[0],
                camera.position.y - instance.position[1],
                camera.position.z - instance.position[2]));


            // Define the up vector
            bx::Vec3 up = { 0.0f, 1.0f, 0.0f };

            // Calculate the right vector (orthogonal to forward and up)
            bx::Vec3 right = bx::normalize(bx::cross(up, modelToCamera));

            // Recalculate the up vector to ensure orthogonality
            up = bx::normalize(bx::cross(modelToCamera, right));

            // Build the model matrix with the orientation facing the camera
            model[0] = right.x;   model[1] = right.y;   model[2] = right.z;   model[3] = 0.0f;
            model[4] = up.x;      model[5] = up.y;      model[6] = up.z;      model[7] = 0.0f;
            model[8] = modelToCamera.x; model[9] = modelToCamera.y; model[10] = modelToCamera.z; model[11] = 0.0f;
            model[12] = instance.position[0];
            model[13] = instance.position[1];
            model[14] = instance.position[2];
            model[15] = 1.0f;
            bgfx::setTransform(model);

            bgfx::setVertexBuffer(0, instance.vertexBuffer);
            bgfx::setIndexBuffer(instance.indexBuffer);
            bgfx::submit(0, program);
        }

        // Update your vertex layout to include normals
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
            .end();



        // Set vertex and index buffers
        //bgfx::setVertexBuffer(0, vbh_cube);
        //bgfx::setIndexBuffer(ibh_cube);
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        // In your render loop, before bgfx::submit:
        float lightDir[4] = { 0.0f, 1.0f, 1.0f, 0.0f }; 
        float lightColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f }; // White light
		float viewPos[4] = { camera.position.x, camera.position.y, camera.position.z, 1.0f };

        bgfx::setUniform(u_lightDir, lightDir);
        bgfx::setUniform(u_lightColor, lightColor);
		bgfx::setUniform(u_viewPos, viewPos);


        // Submit draw call
        bgfx::submit(0, program);
        
        bx::mtxSRT(mtx, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        bgfx::setTransform(mtx);

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

    for (const auto& instance : instances)
    {
        bgfx::destroy(instance.vertexBuffer);
        bgfx::destroy(instance.indexBuffer);
    }

    // Cleanup
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    //bgfx::destroy(program);

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}