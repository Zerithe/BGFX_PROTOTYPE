# BGFX_PROTOTYPE

## DESCRIPTION

BGFX_PROTOTYPE is a simple project that uses bgfx and glfw to render cross hatching shaders with editing functions.

### Working build can be found at:

[Google Drive](https://drive.google.com/drive/u/1/folders/1B-ybRA0cXFT7n0vSWxDYO7zPv6f0cQ1i)

## SETUP 

### bgfx.cmake

- inside the root directory input the following commands:
```
git clone https://github.com/bkaradzic/bgfx.cmake.git
cd bgfx.cmake
git submodule init
git submodule update
cmake -S. -Bcmake-build
cmake --build cmake-build
```

### glfw

- inside the root directory input the following commands:
```
git clone https://github.com/glfw/glfw
cd glfw
cmake -S. -Bcmake-build
cmake --build cmake-build
```

### imgui

- inside the root directory input the following commands:
```
git clone https://github.com/ocornut/imgui
```

### run through VS2022

*if setup does not work, use working build in the google drive link*