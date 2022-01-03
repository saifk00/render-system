cmake_minimum_required(VERSION 3.12...3.22)

project(
    RenderSystem
    DESCRIPTION "A basic OpenGL render system"
    LANGUAGES CXX C
  )

# build external dependencies

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_LIBRARY_DIRS "extern/assimp/lib")

link_directories(${ASSIMP_LIBRARY_DIRS})

add_subdirectory(extern/assimp)
add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)


# build the rendering library
add_subdirectory(src)

# build main
add_subdirectory(apps)

