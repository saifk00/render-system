cmake_minimum_required(VERSION 3.12...3.22)

project(
    RenderSystem
    DESCRIPTION "A basic OpenGL render system"
    LANGUAGES CXX C
  )

# build external dependencies

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_LIBRARY_DIRS "extern/assimp/code")
set(ASSIMP_INCLUDE_DIRS "extern/assimp/include" "extern/assimp/build/include")

include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})

add_subdirectory(extern/assimp)
add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)

# build the rendering library
add_subdirectory(src)

# build main
add_subdirectory(apps)

