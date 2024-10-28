# CMakeList.txt : CMake project for BGFX_PROTOTYPE, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.8)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("BGFX_PROTOTYPE")



# Add source to this project's executable.
add_executable (BGFX_PROTOTYPE
"BGFX_PROTOTYPE.cpp" 
"BGFX_PROTOTYPE.h"   
"InputManager.cpp" 
"InputManager.h" 
"Camera.h" 
"ObjLoader.cpp" 
"ObjLoader.h" 
 "Utility.cpp" 
 "Utility.h")

ADD_CUSTOM_TARGET(
    copy_shader_files
    ${CMAKE_COMMAND}
    -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
    -D DESTINATION_DIR=${CMAKE_CURRENT_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/CopyFile.cmake
    COMMENT "Copying Files for target: BGFX_PROTOTYPE" VERBATIM 
    )

ADD_DEPENDENCIES ( BGFX_PROTOTYPE copy_shader_files )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BGFX_PROTOTYPE PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
add_subdirectory(./bgfx.cmake)
add_subdirectory(./glfw)
target_link_libraries(BGFX_PROTOTYPE PRIVATE bgfx bx bimg glfw imgui)
target_include_directories(BGFX_PROTOTYPE PRIVATE bgfx)
# set(SOURCES "bgfx.cmake/bgfx/examples/common/bgfx_utils.cpp" "bgfx.cmake/bgfx/examples/common/bgfx_utils.h")
# add_library(mylib )

# IMGUI static library creation

set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/imgui)
file(GLOB IMGUI_GLOB
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imconfig.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_internal.h
    ${IMGUI_PATH}/imstb_rectpack.h
    ${IMGUI_PATH}/imstb_textedit.h
    ${IMGUI_PATH}/imstb_truetype.h
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp

    # specific bindings...
    ${IMGUI_PATH}/backends/imgui_impl_glfw.h
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp)
add_library("imgui" STATIC ${IMGUI_GLOB})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})
target_link_libraries("imgui" PRIVATE glfw)

target_include_directories( BGFX_PROTOTYPE PUBLIC imgui)