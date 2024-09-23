# BGFX_PROTOTYPE

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

### run through VS2022

## Goals

- Convert shader files to binary format and load them in the program.
- Convert mesh objects to compatible format and load them in the program.
- ~~Add Inputs for the camera.~~

**Bonus:**
- Add input for adding cubes in the scene.

## Notes

*add here for others to see*

- `Jo` change directory for shader files in main. Don't know why it doesn't work with relative path.

