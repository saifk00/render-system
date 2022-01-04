# Render System

A basic little 3D rendering engine written with OpenGL in C++

Dependencies:
    - glm for transformation math, etc.
    - glad for OpenGL extension loading
    - assimp for loading various texture formats (see [Model.h](include/rendersystem/Model.h))
    - glfw for windowing/input


## Build

`cmake -S. -B_build` should produce the required build files

Note: on linux, you will need development headers for whatever windowing system you use as well as an OpenGL API (`apt install libxorg-dev libgl1-mesa-dev` works for me)

`make *_demo` in the build folder should then create the executable for various demos. The `render_lib` target produces a library containing various utilities for rendering (model loading, lighting, etc.)
