file(GLOB HEADER_LIST "${RenderSystem_SOURCE_DIR}/include/rendersystem/*.h")
file(GLOB SRCS "${RenderSystem_SOURCE_DIR}/src/*.cpp")

#project(Glad)
#add_library(glad "${RenderSystem_SOURCE_DIR}/include/glad/glad.h" "${RenderSystem_SOURCE_DIR}/src/glad.cpp")
#target_include_directories(glad PUBLIC ../include/)
message("render_lib: ${SRCS}")
add_library(render_lib STATIC "${SRCS}" "${HEADER_LIST}")

target_include_directories(render_lib PUBLIC 
    ../include
    ../extern/assimp/include
    ../extern/glfw/include)

target_link_libraries(render_lib PUBLIC glad assimp)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES "${HEADER_LIST}"
)