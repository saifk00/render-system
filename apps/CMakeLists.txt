add_executable(demo_stencil stencil_demo.cpp)
add_executable(demo_blending blending_demo.cpp)
add_executable(demo_advanced advanced_demo.cpp)

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

target_include_directories(demo_stencil PUBLIC ${DEMO_INCLUDES})
target_link_libraries(demo_stencil PRIVATE ${DEMO_LIBS})

target_include_directories(demo_blending PUBLIC ${DEMO_INCLUDES})
target_link_libraries(demo_blending PRIVATE ${DEMO_LIBS})

target_include_directories(demo_advanced PUBLIC ${DEMO_INCLUDES})
target_link_libraries(demo_advanced PRIVATE ${DEMO_LIBS})