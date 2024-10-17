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
add_executable (BGFX_PROTOTYPE "BGFX_PROTOTYPE.cpp" "BGFX_PROTOTYPE.h"   "InputManager.cpp" "InputManager.h" "Camera.h" "ObjLoader.cpp" "ObjLoader.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BGFX_PROTOTYPE PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
add_subdirectory(./bgfx.cmake)
add_subdirectory(./glfw)
target_link_libraries(BGFX_PROTOTYPE PRIVATE bgfx bx bimg glfw)
target_include_directories(BGFX_PROTOTYPE PUBLIC "bgfx.cmake/bgfx/examples/common")