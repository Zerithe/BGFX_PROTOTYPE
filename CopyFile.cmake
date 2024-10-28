# CopyFile.cmake

# Copy a file from the source directory to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaderout DESTINATION ${CMAKE_CURRENT_BINARY_DIR})