add_executable(render_app main.cpp)

set(CMAKE_MODELS_DIR "${RenderSystem_SOURCE_DIR}/models/")
set(CMAKE_ASSETS_DIR "${RenderSystem_SOURCE_DIR}/assets/")
set(CMAKE_SHADERS_DIR "${RenderSystem_SOURCE_DIR}/shaders/")

configure_file(main.h.in
    "${PROJECT_BINARY_DIR}/include/main.h"
    @ONLY)

target_include_directories(render_app PUBLIC
    ${PROJECT_BINARY_DIR}/include
    ../include
    ../extern/assimp/include
    ../extern/glfw/include
)

target_link_libraries(render_app PRIVATE
    render_lib
    glfw
    glad
    assimp)