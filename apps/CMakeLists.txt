add_executable(stencil_demo stencil_demo.cpp)

set(CMAKE_MODELS_DIR "${RenderSystem_SOURCE_DIR}/models/")
set(CMAKE_ASSETS_DIR "${RenderSystem_SOURCE_DIR}/assets/")
set(CMAKE_SHADERS_DIR "${RenderSystem_SOURCE_DIR}/shaders/")

configure_file(resources.h.in
    "${PROJECT_BINARY_DIR}/include/resources.h"
    @ONLY)

set(DEMO_INCLUDES ${PROJECT_BINARY_DIR}/include
    ../include
    ../extern/assimp/include
    ../extern/glfw/include)

set(DEMO_LIBS render_lib glfw glad assimp)

target_include_directories(stencil_demo PUBLIC ${DEMO_INCLUDES})
target_link_libraries(stencil_demo PRIVATE ${DEMO_LIBS})